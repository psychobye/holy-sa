package com.lit.launcher.storage;

import static com.lit.launcher.config.Config.NATIVE_SETTINGS_FILE_PATH;

import android.content.Context;
import android.os.Environment;
import android.widget.Toast;

import com.lit.launcher.config.Config;

import org.ini4j.InvalidFileFormatException;
import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;

public class NativeStorage {

    private static final String CLIENT_SECTION_NAME = "client";

    public static void addClientProperty(String propertyName, String value, Context context) {
        try {
            File dir = new File(Config.GAME_PATH);
            File f = new File(dir, NATIVE_SETTINGS_FILE_PATH);

            if (!f.exists()) {
                return;
            }

            Wini w = new Wini(new File(String.valueOf(f)));
            w.put(CLIENT_SECTION_NAME, propertyName, value);
            w.store();
        } catch (InvalidFileFormatException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static String getClientProperty(String property, Context context) {
        String value = null;

        File dir = new File(Config.GAME_PATH);
        File f = new File(dir, NATIVE_SETTINGS_FILE_PATH);

        try {
            Wini w = new Wini(new File(String.valueOf(f)));
            value = w.get(CLIENT_SECTION_NAME, property);
            w.store();
        } catch (IOException ignored) {

        }

        return value;
    }

    private static void showMessage(String message, Context context) {
        Toast.makeText(context, message, Toast.LENGTH_SHORT)
                .show();
    }
}
