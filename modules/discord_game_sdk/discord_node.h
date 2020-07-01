#ifndef DISCORD_GAME_SDK_H
#define DISCORD_GAME_SDK_H

/**
	@autor HazmatDemon
*/

#include "scene/main/node.h"
#include "core/class_db.h"

#include "modules/discord_game_sdk/source/core.h"
#include "modules/discord_game_sdk/source/ffi.h"

class DiscordSDK : public Node {
    
    GDCLASS(DiscordSDK, Node);

    discord::Core *core{};
	discord::Activity activity{};

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


protected:

    static void _bind_methods();
	void _notification(int p_what);

public:

	Result create(int64_t client_id, CreateFlags flag);
	Result update_activity();

    void set_time_left(int64_t stamp);
	int64_t get_time_left();

	void set_state(const String &state);
    void set_details(const String& detail);

	//String get_user_name(int64_t user_id);

	String get_large_image();
	void set_large_image(const String& image);
	
    DiscordSDK();
    ~DiscordSDK();
};

VARIANT_ENUM_CAST(DiscordSDK::Result);
VARIANT_ENUM_CAST(DiscordSDK::CreateFlags);

#endif // DISCORD_GAME_SDK_H
