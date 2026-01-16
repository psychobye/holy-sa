package com.holy.launcher.storage;

import static com.holy.game.core.Samp.activity;

import android.content.Context;
import android.content.SharedPreferences;

import com.holy.launcher.domain.enums.StorageElements;

import org.apache.commons.lang3.StringUtils;

public class Storage {
    public static final String STORAGE_NAME = "Russia";

    public static boolean getBoolean(String value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(value, false);
    }

    public static int getInt(String value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(value, 0);
    }

    public static void setBoolean(String element, boolean value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putBoolean(element, value);
        editor.apply();
    }
    public static void setInt(String element, int value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putInt(element, value);
        editor.apply();
    }

    public static int getInt(Context context, String value) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(value, 0);
    }

    public static void setInt(Context context, String element, int value) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putInt(element, value);
        editor.apply();
    }

    public static void addProperty(StorageElements element, String value, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putString(element.getValue(), value);
        editor.apply();
    }

    public static void addProperty(StorageElements element, Boolean value, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putBoolean(element.getValue(), value);
        editor.apply();
    }

    public static String getProperty(StorageElements element, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(element.getValue(), null);
    }

    public static Boolean getBooleanProperty(StorageElements element, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(element.getValue(), true);
    }

    public static void deleteProperty(String name, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        for (String key : sharedPreferences.getAll().keySet()) {
            if (key.startsWith(name)) {
                editor.remove(key);
            }
        }

        editor.apply();
    }

    public static boolean isPropertyExist(String name, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        String property = sharedPreferences.getString(name, null);

        return StringUtils.isNotBlank(property);
    }
}
