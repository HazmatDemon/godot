#include "discord_node.h"
#include <iostream>

/**
	@autor HazmatDemon
*/

DiscordSDK::Result DiscordSDK::create(int64_t client_id, DiscordSDK::CreateFlags flag) {
	return static_cast<DiscordSDK::Result>(discord::Core::Create(client_id, flag, &core));
}

DiscordSDK::Result DiscordSDK::update_activity() {
	if (core)
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) { return static_cast<DiscordSDK::Result>(result); });
	return DiscordSDK::Result::OK;
}
int64_t DiscordSDK::get_time_left() {
	return activity.GetTimestamps().GetEnd();
}

void DiscordSDK::set_time_left(int64_t stamp) {
    activity.GetTimestamps().SetEnd(stamp);
}

void DiscordSDK::set_state(const String& state) {
	activity.SetState(state.utf8().ptr());
}

void DiscordSDK::set_details(const String& detail) {
    activity.SetDetails(detail.utf8().ptr());
}

/*
String DiscordSDK::get_user_name(int64_t user_id) {
	String username;
	core->UserManager().GetUser(user_id, [&](discord::Result result, discord::User user) {
		username = String(user.GetUsername());
		return;
	});
	return username;
}
*/

String DiscordSDK::get_large_image() {
	return String(activity.GetAssets().GetLargeImage());
}

void DiscordSDK::set_large_image(const String& image) {
	activity.GetAssets().SetLargeImage(image.utf8().ptrw());
}

void DiscordSDK::_notification(int p_what) {
	if (p_what == NOTIFICATION_PROCESS) {
		if (core)
			core->RunCallbacks();
	} else if (p_what == NOTIFICATION_READY)
		set_process(true);
}

void DiscordSDK::_bind_methods() {
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
	ClassDB::bind_method(D_METHOD("create", "client_id", "create_flag"), &DiscordSDK::create);

    ClassDB::bind_method(D_METHOD("update_activity"), &DiscordSDK::update_activity);
    ClassDB::bind_method(D_METHOD("set_time_left", "stamp"), &DiscordSDK::set_time_left);
	ClassDB::bind_method(D_METHOD("get_time_left"), &DiscordSDK::get_time_left);
	ClassDB::bind_method(D_METHOD("set_state", "state"), &DiscordSDK::set_state);
    ClassDB::bind_method(D_METHOD("set_details", "detail"), &DiscordSDK::set_details);
	//ClassDB::bind_method(D_METHOD("get_user_name", "user_id"), &DiscordSDK::get_user_name);

	ClassDB::bind_method(D_METHOD("get_large_image"), &DiscordSDK::get_large_image);
	ClassDB::bind_method(D_METHOD("set_large_image", "image"), &DiscordSDK::set_large_image);
}

DiscordSDK::DiscordSDK() {
	activity.GetAssets().SetLargeImage("as");
}

DiscordSDK::~DiscordSDK() {
}
