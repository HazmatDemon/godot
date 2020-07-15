/*************************************************************************/
/*  discord_game_sdk.h                                                   */
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

#ifndef DISCORD_GAME_SDK_H
#define DISCORD_GAME_SDK_H

#include "scene/main/node.h"
#include "core/class_db.h"
#include "core/error_macros.h"

#include <core.h>
#include <ffi.h>

class DiscordGameSDK : public Node {

    GDCLASS(DiscordGameSDK, Node);

public:
	enum Result {
		OK = DiscordResult_Ok,
		SERVICE_UNAVAILABLE = DiscordResult_ServiceUnavailable,
		INVALID_VERSION = DiscordResult_InvalidVersion,
		LOCK_FAILED = DiscordResult_LockFailed,
		INTERNAL_ERROR = DiscordResult_InternalError,
		INVALID_PAYLOAD = DiscordResult_InvalidPayload,
		INVALID_COMMAND = DiscordResult_InvalidCommand,
		INVALID_PERMISSION = DiscordResult_InvalidPermissions,
		NOT_FETCHED = DiscordResult_NotFetched,
		NOT_FOUND = DiscordResult_NotFound,
		CONFLICT = DiscordResult_Conflict,
		INVALID_SECRET = DiscordResult_InvalidSecret,
		INVALID_JOIN_SECRET = DiscordResult_InvalidJoinSecret,
		NO_ELIGIBLE_ACTIVITY = DiscordResult_NoEligibleActivity,
		INVALID_INVITE = DiscordResult_InvalidInvite,
		NOT_AUTHENTICATED = DiscordResult_NotAuthenticated,
		INVALIS_ACCESS_TOKEN = DiscordResult_InvalidAccessToken,
		APPLICATION_MISMATCH = DiscordResult_ApplicationMismatch,
		INVALID_DATA_URL = DiscordResult_InvalidDataUrl,
		INVALID_BASE_64 = DiscordResult_InvalidBase64,
		NOT_FILTERED = DiscordResult_NotFiltered,
		LOBBY_FULL = DiscordResult_LobbyFull,
		INVALID_LOBBY_SECRET = DiscordResult_InvalidLobbySecret,
		INVALID_FILE_NAME = DiscordResult_InvalidFilename,
		INVALID_FILE_SIZE = DiscordResult_InvalidFileSize,
		INVALID_ENTITLEMENT = DiscordResult_InvalidEntitlement,
		NOT_INSTALLED = DiscordResult_NotInstalled,
		NOT_RUNNING = DiscordResult_NotRunning,
		INSUFFICIENT_BUFFER = DiscordResult_InsufficientBuffer,
		PURCHASE_CANCELED = DiscordResult_PurchaseCanceled,
		INVALID_GUILD = DiscordResult_InvalidGuild,
		INVALID_EVENT = DiscordResult_InvalidEvent,
		INVALID_CHANNEL = DiscordResult_InvalidChannel,
		INAVLID_ORIGIN = DiscordResult_InvalidOrigin,
		RATE_LIMIT = DiscordResult_RateLimited,
		OAUTH2_ERROR = DiscordResult_OAuth2Error,
		SELCT_CHANNEL_TIMEOUT = DiscordResult_SelectChannelTimeout,
		GET_GUILD_TIMEOUT = DiscordResult_GetGuildTimeout,
		SELECT_VOICE_FORCE_REQUIRED = DiscordResult_SelectVoiceForceRequired,
		CAPTURE_SHORTCUT_ALREADY_LISTENING = DiscordResult_CaptureShortcutAlreadyListening,
		UNAUTHORIZED_FOR_ACHIEVEMENTS = DiscordResult_UnauthorizedForAchievement,
		INVALID_GIFT_CODE = DiscordResult_InvalidGiftCode,
		PURCHASE_ERROR = DiscordResult_PurchaseError,
		TRANSCATION_ABORTED = DiscordResult_TransactionAborted

	};

	enum CreateFlags {
		DEFAULT = DiscordCreateFlags_Default,
		NO_REQUIRE_DISCORD = DiscordCreateFlags_NoRequireDiscord

	};

	enum LobbyType {
		PRIVATE = DiscordLobbyType_Private,
		PUBLIC = DiscordLobbyType_Public

	};

	enum LobbySearchComparison {
		LESS_THAN_OR_EQUAL = DiscordLobbySearchComparison_LessThanOrEqual,
		LESS_THAN = DiscordLobbySearchComparison_LessThan,
		EQUAL = DiscordLobbySearchComparison_Equal,
		GREATER_THAN = DiscordLobbySearchComparison_GreaterThan,
		GREATER_THAN_OR_EQUAL = DiscordLobbySearchComparison_GreaterThanOrEqual,
		NOT_EQUAL = DiscordLobbySearchComparison_NotEqual

	};

	enum LobbySearchCast {
		STRING = DiscordLobbySearchCast_String,
		NUMBER = DiscordLobbySearchCast_Number

	};

private:
	discord::Core *core{};

	discord::Activity activity{};

	void _lobby_created(Result result, int64_t lobby_id, int64_t owner_id, const String& secret, bool is_locked);
	void _lobbies_found(Result result, int32_t count);
	void _lobby_connected(Result result);
	void _voice_connected(Result result);

protected:
	void _notification(int p_what);
    static void _bind_methods();

public:
	Result create(int64_t client_id, CreateFlags flag);

	Result update_activity() const;

    void set_time_left(int64_t stamp);
	int64_t get_time_left() const;

	void set_state(const String &state);
	String get_state() const;

    void set_details(const String& detail);
	String get_details() const;

	void set_large_image(const String& image);
	String get_large_image() const;
	
	void create_lobby(const String& name, int capacity, LobbyType type);

	void find_lobbies(const String& key, LobbySearchComparison comp, LobbySearchCast cast, const String& value, uint32_t count);
	void connect_lobby(int64_t lobby_id, const String& secret);
	void connect_voice(int64_t lobby_id);

    DiscordGameSDK();
    ~DiscordGameSDK();
};

VARIANT_ENUM_CAST(DiscordGameSDK::Result);
VARIANT_ENUM_CAST(DiscordGameSDK::CreateFlags);
VARIANT_ENUM_CAST(DiscordGameSDK::LobbyType);
VARIANT_ENUM_CAST(DiscordGameSDK::LobbySearchComparison);
VARIANT_ENUM_CAST(DiscordGameSDK::LobbySearchCast)

#endif // DISCORD_GAME_SDK_H
