package com.lit.launcher.service;

import android.app.Activity;

public interface ActivityService {

    void showInfoMessage(String message, Activity activity);

    boolean isGameFileInstall(Activity activity, String filePath);

    void showBigMessage(String message, Activity activity);
}
