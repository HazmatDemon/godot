
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

void DiscordGameSDK::_lobby_created(DiscordGameSDK::Result result, int64_t lobby_id, int64_t owner_id, const String& secret, bool is_locked) {

	emit_signal("lobby_created", result, lobby_id, owner_id, secret, is_locked);
}

void DiscordGameSDK::_lobbies_found(DiscordGameSDK::Result result, int32_t count) {

	emit_signal("lobbies_found", result, count);
}

void DiscordGameSDK::_lobby_connected(DiscordGameSDK::Result result) {

	emit_signal("lobby_connected", result);
}

void DiscordGameSDK::_voice_connected(Result result) {

	emit_signal("voice_connected", result);
}

DiscordGameSDK::Result DiscordGameSDK::create(int64_t client_id, DiscordGameSDK::CreateFlags flag) {
	return static_cast<DiscordGameSDK::Result>(discord::Core::Create(client_id, flag, &core));
}

DiscordGameSDK::Result DiscordGameSDK::update_activity() const {

	ERR_FAIL_COND_V(!core, DiscordGameSDK::Result::INTERNAL_ERROR);

	DiscordGameSDK::Result ret;

	core->ActivityManager().UpdateActivity(activity, [&](discord::Result result) { 
		ret = static_cast<DiscordGameSDK::Result>(result); 
	});
	return ret;
}

void DiscordGameSDK::set_time_left(int64_t stamp) {

	activity.GetTimestamps().SetEnd(stamp);
}

int64_t DiscordGameSDK::get_time_left() const {
	return activity.GetTimestamps().GetEnd();
}

void DiscordGameSDK::set_state(const String& state) {

	activity.SetState(state.utf8().ptr());
}

String DiscordGameSDK::get_state() const {
	return activity.GetState();
}

void DiscordGameSDK::set_details(const String& detail) {

    activity.SetDetails(detail.utf8().ptr());
}

String DiscordGameSDK::get_details() const {
	return activity.GetDetails();
}

void DiscordGameSDK::set_large_image(const String& image) {

	activity.GetAssets().SetLargeImage(image.utf8().ptrw());
}

String DiscordGameSDK::get_large_image() const {
	return String(activity.GetAssets().GetLargeImage());
}

void DiscordGameSDK::create_lobby(const String& name, int capacity, DiscordGameSDK::LobbyType type) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager& lobby_manager = core->LobbyManager();
	discord::LobbyTransaction transaction;

	lobby_manager.GetLobbyCreateTransaction(&transaction);

	transaction.SetType(static_cast<discord::LobbyType>(type));
	transaction.SetCapacity(capacity);
	transaction.SetMetadata("name", name.utf8().ptr());

	lobby_manager.CreateLobby(transaction, [&](discord::Result result, discord::Lobby lobby) {
		call_deferred("_lobby_created", static_cast<DiscordGameSDK::Result>(result), lobby.GetId(), lobby.GetOwnerId(), String(lobby.GetSecret()), lobby.GetLocked());
	});
}

void DiscordGameSDK::find_lobbies(const String& key, DiscordGameSDK::LobbySearchComparison comp, DiscordGameSDK::LobbySearchCast cast, const String& value, uint32_t limit) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager& lobby_manager = core->LobbyManager();
	discord::LobbySearchQuery query;
	lobby_manager.GetSearchQuery(&query);
	query.Filter(key.utf8().ptr(), static_cast<discord::LobbySearchComparison>(comp), static_cast<discord::LobbySearchCast>(cast), value.utf8().ptr());
	query.Limit(limit);
	lobby_manager.Search(query, [&](discord::Result result) {
		int32_t count;
		lobby_manager.LobbyCount(&count);
		call_deferred("_lobbies_found", static_cast<DiscordGameSDK::Result>(result), count);
	});
}

void DiscordGameSDK::connect_lobby(int64_t lobby_id, const String& secret) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager& lobby_manager = core->LobbyManager();

	lobby_manager.ConnectLobby(lobby_id, secret.utf8().ptr(), [&](discord::Result result, discord::Lobby lobby) {
		call_deferred("_lobby_connected", static_cast<DiscordGameSDK::Result>(result));
	});
}

void DiscordGameSDK::connect_voice(int64_t lobby_id) {

	ERR_FAIL_COND(!core);

	discord::LobbyManager& lobby_manager = core->LobbyManager();

	lobby_manager.ConnectVoice(lobby_id, [&](discord::Result result) {
		emit_signal("_voice_connected", static_cast<DiscordGameSDK::Result>(result));
	});
}

void DiscordGameSDK::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_READY: {

			set_process(true);
		} break;
		case NOTIFICATION_PROCESS: {

			if (!core) {
				ERR_PRINT_ONCE("Core is uninitialized");
				return;
			}
			core->RunCallbacks();
		} break;
	}
}

void DiscordGameSDK::_bind_methods() {

	ClassDB::bind_method(D_METHOD("create", "client_id", "create_flag"), &DiscordGameSDK::create);

    ClassDB::bind_method(D_METHOD("update_activity"), &DiscordGameSDK::update_activity);

    ClassDB::bind_method(D_METHOD("set_time_left", "stamp"), &DiscordGameSDK::set_time_left);
	ClassDB::bind_method(D_METHOD("get_time_left"), &DiscordGameSDK::get_time_left);

	ClassDB::bind_method(D_METHOD("set_state", "state"), &DiscordGameSDK::set_state);
	ClassDB::bind_method(D_METHOD("get_state"), &DiscordGameSDK::get_state);

    ClassDB::bind_method(D_METHOD("set_details", "detail"), &DiscordGameSDK::set_details);
	ClassDB::bind_method(D_METHOD("get_detials"), &DiscordGameSDK::get_details);

	ClassDB::bind_method(D_METHOD("set_large_image", "image"), &DiscordGameSDK::set_large_image);
	ClassDB::bind_method(D_METHOD("get_large_image"), &DiscordGameSDK::get_large_image);

	ClassDB::bind_method(D_METHOD("create_lobby", "name", "capacity", "type"), &DiscordGameSDK::create_lobby);
	ClassDB::bind_method(D_METHOD("find_lobbies", "key", "comp", "cast", "value", "count"), &DiscordGameSDK::find_lobbies);
	ClassDB::bind_method(D_METHOD("connect_lobby", "lobby_id", "secret"), &DiscordGameSDK::connect_lobby);
	ClassDB::bind_method(D_METHOD("connect_voice", "lobby_id"), &DiscordGameSDK::connect_voice);

	ClassDB::bind_method(D_METHOD("_lobby_created", "result", "lobby_id", "owner_id", "secret", "is_locked"), &DiscordGameSDK::_lobby_created);
	ClassDB::bind_method(D_METHOD("_lobbies_found", "result", "count"), &DiscordGameSDK::_lobbies_found);
	ClassDB::bind_method(D_METHOD("_lobby_connected", "result"), &DiscordGameSDK::_lobby_connected);
	ClassDB::bind_method(D_METHOD("_voice_connected", "result"), &DiscordGameSDK::_voice_connected);
	

	ADD_SIGNAL(MethodInfo("lobby_created", PropertyInfo(Variant::INT, "result"), PropertyInfo(Variant::INT, "lobby_id"), PropertyInfo(Variant::INT, "owner_id"), PropertyInfo(Variant::STRING, "secret"), PropertyInfo(Variant::BOOL, "is_locked")));
	ADD_SIGNAL(MethodInfo("lobbies_found", PropertyInfo(Variant::INT, "result"), PropertyInfo(Variant::INT, "count")));
	ADD_SIGNAL(MethodInfo("lobby_connected", PropertyInfo(Variant::INT, "result")));
	ADD_SIGNAL(MethodInfo("voice_connected", PropertyInfo(Variant::INT, "result")));

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
	BIND_ENUM_CONSTANT(INVALIS_ACCESS_TOKEN);
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
	BIND_ENUM_CONSTANT(SELCT_CHANNEL_TIMEOUT);
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

DiscordGameSDK::DiscordGameSDK() {
}

DiscordGameSDK::~DiscordGameSDK() {
	delete core;
}
