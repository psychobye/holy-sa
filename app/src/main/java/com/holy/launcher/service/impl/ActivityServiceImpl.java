package com.holy.launcher.service.impl;

import android.app.Activity;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.lit.game.R;
import com.holy.launcher.service.ActivityService;

import java.io.File;

public class ActivityServiceImpl implements ActivityService {

    public static void showErrorMessage(String message, Activity activity) {

        LayoutInflater inflater = activity.getLayoutInflater();
        View layout = inflater.inflate(R.layout.toast_error,
                (ViewGroup) activity.findViewById(R.id.toast_layout_root));

        TextView text = (TextView) layout.findViewById(R.id.br_not_text);
        text.setText(message);

        Toast toast = new Toast(activity.getApplicationContext());
        toast.setGravity(Gravity.BOTTOM, 0, 0);
        toast.setDuration(Toast.LENGTH_LONG);
        toast.setView(layout);
        toast.show();
    }
//
//    @Override
//    public void showErrorMessage(String message, Activity activity) {
//
//        LayoutInflater inflater = activity.getLayoutInflater();
//        View layout = inflater.inflate(R.layout.toast_error,
//                (ViewGroup) activity.findViewById(R.id.toast_layout_root));
//
//        TextView text = (TextView) layout.findViewById(R.id.br_not_text);
//        text.setText(message);
//
//        Toast toast = new Toast(activity.getApplicationContext());
//        toast.setGravity(Gravity.BOTTOM, 0, 0);
//        toast.setDuration(Toast.LENGTH_LONG);
//        toast.setView(layout);
//        toast.show();
//    }

    @Override
    public void showInfoMessage(String message, Activity activity) {
        LayoutInflater inflater = activity.getLayoutInflater();
        View layout = inflater.inflate(R.layout.toast_inform, activity.findViewById(R.id.toast_layout_root));

        TextView text = layout.findViewById(R.id.br_not_text);
        text.setText(message);

        Toast toast = new Toast(activity.getApplicationContext());
        toast.setGravity(Gravity.BOTTOM, 0, 0);
        toast.setDuration(Toast.LENGTH_LONG);
        toast.setView(layout);
        toast.show();
    }

    @Override
    public void showBigMessage(String message, Activity activity) {
        Toast.makeText(activity, message, Toast.LENGTH_LONG)
                .show();
    }

    @Override
    public boolean isGameFileInstall(Activity activity, String filePath) {
        String fileFullPath = activity.getExternalFilesDir(null).toString()
                .concat(filePath);

        File file = new File(fileFullPath);
        return file.exists();
    }
}
