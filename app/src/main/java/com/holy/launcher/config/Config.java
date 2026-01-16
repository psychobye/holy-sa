package com.holy.launcher.config;

import com.holy.launcher.NetworkService;

public class Config {
    public static final String APK_FILE_NAME = "russia.apk";
    public static final String GAME_PATH = "/storage/emulated/0/TESTLIT";
    public static final String ZIP_FILES_BASE_ADR = NetworkService.FILES_BASE_ADR + "zip/";
    public static final String FILE_INFO_URL = NetworkService.FILES_BASE_ADR + "files.php";

    public static final String URL_RE_CAPTCHA = "https://files.russia.online/reCaptcha.html";
    private static final String URL_DONATE = "https://russia.online/donate_v2/confirm.php?server=%s&serverName=%s&sum=%s&account=%s&mail=%s&captcha=%s";

    public static final String LIVE_RUSSIA_RESOURCE_SERVER_URL = "https://files.russia.online";
    public static final String FORUM_URL = "https://t.me/hunteyes";
    public static final String DONATE_URL = "https://russia.online/donate/";

    public static final String SUPPORT_MAIL = "tayksan1337@gmail.com";

    public static final String DISCORD_URI = "https://discord.com/invite/pkT6SEEXKS";
    public static final String VK_URI = "https://vk.com/russia.online";
    public static final String SUPPORT_URI = "https://t.me/m/YcpsYkPTOWU6";
    public static final String YOUTUBE_URI = "https://www.youtube.com/@salagastalone";
    public static final String TELEGRAM_URI = "https://t.me/hunteyes";

    public static final String NATIVE_SETTINGS_FILE_PATH = "/SAMP/settings.ini";
    public static final String SETTINGS_FILE_PATH = "/gta_sa.set";
    public static final String LOG_FILE_PATH = "/log.txt";
    public static final String CRASH_LOG_FILE_PATH = "/crash_log.txt";

    public static String createBillingUri(String serverId, String serverName, String sum, String nickname, String mail, String captcha) {
        return String.format(
                URL_DONATE,
                serverId,
                serverName,
                sum,
                nickname,
                mail,
                captcha
        );
    }
}