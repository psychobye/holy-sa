package com.holy.launcher;

import static com.holy.launcher.config.Config.FILE_INFO_URL;

import com.holy.launcher.async.dto.response.GameFileInfoDto;
import com.holy.launcher.async.dto.response.LatestVersionInfoDto;
import com.holy.launcher.async.dto.response.LoaderSliderInfoResponseDto;
import com.holy.launcher.async.dto.response.MonitoringData;

import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Headers;

public interface NetworkService {

    String FILES_BASE_ADR = "https://files.liverussia.online/gamecache_test/";
    String APK_URL = "https://files.liverussia.online/apk/test/app-ver_test-release.apk";

    @Headers("Content-Type: application/json")
    @GET("https://api.liverussia.online/old_data_test.php")
    Call<MonitoringData> getMonitoringData();

    @Headers("Content-Type: application/json")
    @GET(FILE_INFO_URL)
    Call<GameFileInfoDto> getFilesList();

    @Headers("Content-Type: application/json")
    @GET("https://files.liverussia.online/loader_slider/texts.json")
    Call<LoaderSliderInfoResponseDto> getLoaderSliderInfo();

    @Headers("Content-Type: application/json")
    @GET("https://files.liverussia.online/apk/test/apk_info.php")
    Call<LatestVersionInfoDto> getLatestVersionInfoDto();
}
