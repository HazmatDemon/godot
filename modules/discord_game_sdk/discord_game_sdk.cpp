/*************************************************************************/
/*  discord_game_sdk.cpp                                                 */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "discord_game_sdk.h"

void DiscordGameSDK::_activity_updated(DiscordGameSDK::Result result) {

	emit_signal("activity_updated", result);
}

void DiscordGameSDK::_activity_cleared(DiscordGameSDK::Result result) {

	emit_signal("activity_cleared", result);
}

void DiscordGameSDK::_lobby_created(DiscordGameSDK::Result result, int64_t lobby_id, int64_t owner_id, const String &secret, bool is_locked) {

	emit_signal("lobby_created", result, lobby_id, owner_id, secret, is_locked);
}

void DiscordGameSDK::_lobby_type_updated(DiscordGameSDK::Result result, int64_t lobby_id) {

	emit_signal("lobby_type_updated", result, lobby_id);
}

void DiscordGameSDK::_lobby_get_type(DiscordGameSDK::Result result, int64_t lobby_id, DiscordGameSDK::LobbyType lobby_type) {

	emit_signal("lobby_get_type", result, lobby_id, lobby_type);
}

void DiscordGameSDK::_connected_to_lobby(DiscordGameSDK::Result result) {

	emit_signal("connected_to_lobby", result);
}

void DiscordGameSDK::_connected_to_lobby_voice(Result result) {

	emit_signal("connected_to_lobby_voice", result);
}

DiscordGameSDK::Result DiscordGameSDK::create(int64_t client_id, DiscordGameSDK::CreateFlags flag) {
	return static_cast<DiscordGameSDK::Result>(discord::Core::Create(client_id, flag, &core));
}

void DiscordGameSDK::update_activity() {

	ERR_FAIL_COND_MSG(!core, "You need to call \"DiscordGameSDK::create\" before updating activity. Also make sure it was successfully created");

	core->ActivityManager().UpdateActivity(activity, [&](discord::Result result) {
		call_deferred("_activity_updated", static_cast<DiscordGameSDK::Result>(result));
	});
}

void DiscordGameSDK::clear_activity() {

	ERR_FAIL_COND_MSG(!core, "You need to call \"DiscordGameSDK::create\" before updating activity. Also make sure it was successfully created");

	core->ActivityManager().ClearActivity([&](discord::Result result) {
		call_deferred("_activity_cleared", static_cast<DiscordGameSDK::Result>(result));
	});
}

void DiscordGameSDK::set_activity_time_left(int64_t stamp) {

	activity.GetTimestamps().SetEnd(stamp);
}

int64_t DiscordGameSDK::get_activity_time_left() const {
	return activity.GetTimestamps().GetEnd();
}

void DiscordGameSDK::set_activity_time_elapsed(int64_t stamp) {

	activity.GetTimestamps().SetStart(stamp);
}

int64_t DiscordGameSDK::get_activity_time_elapsed() const {
	return activity.GetTimestamps().GetStart();
}

void DiscordGameSDK::set_activity_state(const String &state) {

	activity.SetState(state.utf8().ptr());
}

String DiscordGameSDK::get_activity_state() const {
	return activity.GetState();
}

void DiscordGameSDK::set_activity_details(const String &detail) {

	activity.SetDetails(detail.utf8().ptr());
}

String DiscordGameSDK::get_activity_details() const {
	return activity.GetDetails();
}

void DiscordGameSDK::set_activity_large_image(const String &image) {

	activity.GetAssets().SetLargeImage(image.utf8().ptrw());
}

String DiscordGameSDK::get_activity_large_image() const {
	return String(activity.GetAssets().GetLargeImage());
}

void DiscordGameSDK::create_lobby(const String &name, uint32_t capacity, DiscordGameSDK::LobbyType type) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager &lobby_manager = core->LobbyManager();

	discord::LobbySearchQuery search_query;
	DiscordGameSDK::Result result = static_cast<DiscordGameSDK::Result>(lobby_manager.GetSearchQuery(&search_query));

	if (result != DiscordGameSDK::Result::OK) {
		return;
	}
	search_query.Filter("metadata.name", discord::LobbySearchComparison::Equal, discord::LobbySearchCast::String, name.utf8().ptr());

	lobby_manager.Search(search_query, [&](discord::Result result) {
		int32_t count;
		lobby_manager.LobbyCount(&count);

		if (count >= 0 && count < 1) {
			discord::LobbyTransaction transaction;

			lobby_manager.GetLobbyCreateTransaction(&transaction);

			transaction.SetType(static_cast<discord::LobbyType>(type));
			transaction.SetCapacity(capacity);
			transaction.SetMetadata("name", name.utf8().ptr());

			lobby_manager.CreateLobby(transaction, [&](discord::Result result, discord::Lobby lobby) {
				call_deferred("_lobby_created", static_cast<DiscordGameSDK::Result>(result), lobby.GetId(), lobby.GetOwnerId(), String(lobby.GetSecret()), lobby.GetLocked());
			});
		}
	});
}

void DiscordGameSDK::set_lobby_type(int64_t lobby_id, DiscordGameSDK::LobbyType lobby_type) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager &lobby_manager = core->LobbyManager();
	discord::LobbyTransaction lobby_transaction;

	DiscordGameSDK::Result result = static_cast<DiscordGameSDK::Result>(lobby_manager.GetLobbyUpdateTransaction(lobby_id, &lobby_transaction));

	if (result != DiscordGameSDK::Result::OK) {
		call_deferred("_lobby_type_updated", result, lobby_id);
		return;
	}

	lobby_transaction.SetType(static_cast<discord::LobbyType>(lobby_type));

	lobby_manager.UpdateLobby(lobby_id, lobby_transaction, [&](discord::Result result) {
		call_deferred("_lobby_type_updated", static_cast<DiscordGameSDK::Result>(result), lobby_id);
	});
}

void DiscordGameSDK::get_lobby_type(int64_t lobby_id) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager &lobby_manager = core->LobbyManager();
	discord::LobbySearchQuery search_query;

	DiscordGameSDK::Result result = static_cast<DiscordGameSDK::Result>(lobby_manager.GetSearchQuery(&search_query));
	if (result != DiscordGameSDK::Result::OK) {
		call_deferred("_lobby_get_type", result, lobby_id, 0);
		return;
	}

	lobby_manager.Search(search_query, [&](discord::Result result) {
		discord::Lobby lobby;
		lobby_manager.GetLobby(lobby_id, &lobby);
		call_deferred("_lobby_get_type", static_cast<DiscordGameSDK::Result>(result), lobby_id, static_cast<DiscordGameSDK::LobbyType>(lobby.GetType()));
	});
}

void DiscordGameSDK::connect_to_lobby(int64_t lobby_id, const String &secret) {

	ERR_FAIL_COND(!core || secret.empty());

	discord::LobbyManager &lobby_manager = core->LobbyManager();

	lobby_manager.ConnectLobby(lobby_id, secret.utf8().ptr(), [&](discord::Result result, discord::Lobby lobby) {
		call_deferred("_lobby_connected", static_cast<DiscordGameSDK::Result>(result));
	});
}

void DiscordGameSDK::connect_to_lobby_voice(int64_t lobby_id) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager &lobby_manager = core->LobbyManager();

	lobby_manager.ConnectVoice(lobby_id, [&](discord::Result result) {
		call_deferred("_voice_connected", static_cast<DiscordGameSDK::Result>(result));
	});
}

void DiscordGameSDK::_bind_methods() {

	ClassDB::bind_method(D_METHOD("_activity_updated", "result"), &DiscordGameSDK::_activity_updated);
	ClassDB::bind_method(D_METHOD("_activity_cleared", "result"), &DiscordGameSDK::_activity_cleared);
	ClassDB::bind_method(D_METHOD("_lobby_created", "result", "lobby_id", "owner_id", "secret", "is_locked"), &DiscordGameSDK::_lobby_created);

	ClassDB::bind_method(D_METHOD("_lobby_type_updated", "result", "lobby_id"), &DiscordGameSDK::_lobby_type_updated);
	ClassDB::bind_method(D_METHOD("_lobby_get_type", "result", "lobby_id", "lobby_type"), &DiscordGameSDK::_lobby_get_type);

	ClassDB::bind_method(D_METHOD("_connected_to_lobby", "result"), &DiscordGameSDK::_connected_to_lobby);
	ClassDB::bind_method(D_METHOD("_connected_to_lobby_voice", "result"), &DiscordGameSDK::_connected_to_lobby_voice);

	ClassDB::bind_method(D_METHOD("create", "client_id", "create_flag"), &DiscordGameSDK::create);

	ClassDB::bind_method(D_METHOD("update_activity"), &DiscordGameSDK::update_activity);
	ClassDB::bind_method(D_METHOD("clear_activity"), &DiscordGameSDK::clear_activity);

	ClassDB::bind_method(D_METHOD("set_activity_time_left", "stamp"), &DiscordGameSDK::set_activity_time_left);
	ClassDB::bind_method(D_METHOD("get_activity_time_left"), &DiscordGameSDK::get_activity_time_left);

	ClassDB::bind_method(D_METHOD("set_activity_time_elapsed", "stamp"), &DiscordGameSDK::set_activity_time_elapsed);
	ClassDB::bind_method(D_METHOD("get_activity_time_elapsed"), &DiscordGameSDK::get_activity_time_elapsed);

	ClassDB::bind_method(D_METHOD("set_activity_state", "state"), &DiscordGameSDK::set_activity_state);
	ClassDB::bind_method(D_METHOD("get_activity_state"), &DiscordGameSDK::get_activity_state);

	ClassDB::bind_method(D_METHOD("set_activity_details", "detail"), &DiscordGameSDK::set_activity_details);
	ClassDB::bind_method(D_METHOD("get_activity_detials"), &DiscordGameSDK::get_activity_details);

	ClassDB::bind_method(D_METHOD("set_activity_large_image", "image"), &DiscordGameSDK::set_activity_large_image);
	ClassDB::bind_method(D_METHOD("get_activity_large_image"), &DiscordGameSDK::get_activity_large_image);

	ClassDB::bind_method(D_METHOD("create_lobby", "name", "capacity", "type"), &DiscordGameSDK::create_lobby);

	ClassDB::bind_method(D_METHOD("set_lobby_type", "lobby_id", "lobby_type"), &DiscordGameSDK::set_lobby_type);
	ClassDB::bind_method(D_METHOD("get_lobby_type", "lobby_id"), &DiscordGameSDK::get_lobby_type);

	ClassDB::bind_method(D_METHOD("connect_to_lobby", "lobby_id", "secret"), &DiscordGameSDK::connect_to_lobby);
	ClassDB::bind_method(D_METHOD("connect_to_lobby_voice", "lobby_id"), &DiscordGameSDK::connect_to_lobby_voice);

	ADD_SIGNAL(MethodInfo("lobby_created", PropertyInfo(Variant::INT, "result"), PropertyInfo(Variant::INT, "lobby_id"), PropertyInfo(Variant::INT, "owner_id"), PropertyInfo(Variant::STRING, "secret"), PropertyInfo(Variant::BOOL, "is_locked")));
	ADD_SIGNAL(MethodInfo("activity_updated", PropertyInfo(Variant::INT, "result")));
	ADD_SIGNAL(MethodInfo("activity_cleared", PropertyInfo(Variant::INT, "result")));

	ADD_SIGNAL(MethodInfo("lobby_type_updated", PropertyInfo(Variant::INT, "result"), PropertyInfo(Variant::INT, "lobby_id")));
	ADD_SIGNAL(MethodInfo("lobby_get_type", PropertyInfo(Variant::INT, "result"), PropertyInfo(Variant::INT, "lobby_id"), PropertyInfo(Variant::INT, "lobby_type")));

	ADD_SIGNAL(MethodInfo("connected_to_lobby", PropertyInfo(Variant::INT, "result")));
	ADD_SIGNAL(MethodInfo("connected_to_lobby_voice", PropertyInfo(Variant::INT, "result")));

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(SERVICE_UNAVAILABLE);
	BIND_ENUM_CONSTANT(INVALID_VERSION);
	BIND_ENUM_CONSTANT(LOCK_FAILED);
	BIND_ENUM_CONSTANT(INTERNAL_ERROR);
	BIND_ENUM_CONSTANT(INVALID_PAYLOAD);
	BIND_ENUM_CONSTANT(INVALID_COMMAND);
	BIND_ENUM_CONSTANT(INVALID_PERMISSION);
	BIND_ENUM_CONSTANT(NOT_FETCHED);
	BIND_ENUM_CONSTANT(NOT_FOUND);
	BIND_ENUM_CONSTANT(CONFLICT);
	BIND_ENUM_CONSTANT(INVALID_SECRET);
	BIND_ENUM_CONSTANT(INVALID_JOIN_SECRET);
	BIND_ENUM_CONSTANT(NO_ELIGIBLE_ACTIVITY);
	BIND_ENUM_CONSTANT(INVALID_INVITE);
	BIND_ENUM_CONSTANT(NOT_AUTHENTICATED);
	BIND_ENUM_CONSTANT(INVALID_ACCESS_TOKEN);
	BIND_ENUM_CONSTANT(APPLICATION_MISMATCH);
	BIND_ENUM_CONSTANT(INVALID_DATA_URL);
	BIND_ENUM_CONSTANT(INVALID_BASE_64);
	BIND_ENUM_CONSTANT(NOT_FILTERED);
	BIND_ENUM_CONSTANT(LOBBY_FULL);
	BIND_ENUM_CONSTANT(INVALID_LOBBY_SECRET);
	BIND_ENUM_CONSTANT(INVALID_FILE_NAME);
	BIND_ENUM_CONSTANT(INVALID_FILE_SIZE);
	BIND_ENUM_CONSTANT(INVALID_ENTITLEMENT);
	BIND_ENUM_CONSTANT(NOT_INSTALLED);
	BIND_ENUM_CONSTANT(NOT_RUNNING);
	BIND_ENUM_CONSTANT(INSUFFICIENT_BUFFER);
	BIND_ENUM_CONSTANT(PURCHASE_CANCELED);
	BIND_ENUM_CONSTANT(INVALID_GUILD);
	BIND_ENUM_CONSTANT(INVALID_EVENT);
	BIND_ENUM_CONSTANT(INVALID_CHANNEL);
	BIND_ENUM_CONSTANT(INAVLID_ORIGIN);
	BIND_ENUM_CONSTANT(RATE_LIMIT);
	BIND_ENUM_CONSTANT(OAUTH2_ERROR);
	BIND_ENUM_CONSTANT(SELECT_CHANNEL_TIMEOUT);
	BIND_ENUM_CONSTANT(GET_GUILD_TIMEOUT);
	BIND_ENUM_CONSTANT(SELECT_VOICE_FORCE_REQUIRED);
	BIND_ENUM_CONSTANT(CAPTURE_SHORTCUT_ALREADY_LISTENING);
	BIND_ENUM_CONSTANT(UNAUTHORIZED_FOR_ACHIEVEMENTS);
	BIND_ENUM_CONSTANT(INVALID_GIFT_CODE);
	BIND_ENUM_CONSTANT(PURCHASE_ERROR);
	BIND_ENUM_CONSTANT(TRANSCATION_ABORTED);

	BIND_ENUM_CONSTANT(DEFAULT);
	BIND_ENUM_CONSTANT(NO_REQUIRE_DISCORD);

	BIND_ENUM_CONSTANT(PRIVATE);
	BIND_ENUM_CONSTANT(PUBLIC);

	BIND_ENUM_CONSTANT(LESS_THAN_OR_EQUAL);
	BIND_ENUM_CONSTANT(LESS_THAN);
	BIND_ENUM_CONSTANT(EQUAL);
	BIND_ENUM_CONSTANT(GREATER_THAN);
	BIND_ENUM_CONSTANT(GREATER_THAN_OR_EQUAL);
	BIND_ENUM_CONSTANT(NOT_EQUAL);

	BIND_ENUM_CONSTANT(STRING);
	BIND_ENUM_CONSTANT(NUMBER);
}

void DiscordGameSDK::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {

			set_process(true);
		} break;
		case NOTIFICATION_PROCESS: {

			if (core) {
				core->RunCallbacks();
			}
		} break;
		default:
			break;
	}
}

DiscordGameSDK::DiscordGameSDK() {
}

DiscordGameSDK::~DiscordGameSDK() {
	if (core) {
		delete core;
	}
}
