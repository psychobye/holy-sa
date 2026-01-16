package com.holy.game.gui.adapters;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.lit.game.R;

import java.util.List;

public class TheftAutoLevelsAdapter extends ArrayAdapter<Integer> {
    private final Activity activity;
    private List<Integer> dataModalArrayList;

    public TheftAutoLevelsAdapter(@NonNull Activity activity, List<Integer> dataModalArrayList) {
        super(activity, 0, dataModalArrayList);
        this.activity = activity;
        this.dataModalArrayList = dataModalArrayList;
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {

        View item = convertView;
        if (item == null) {
            item = LayoutInflater.from(getContext()).inflate(R.layout.theft_auto_level_item, parent, false);
        }

        int color = getItem(position);
        ImageView imageView = item.findViewById(R.id.theft_auto_level_item_image);
        imageView.setBackgroundResource(color);

        return item;
    }

    public void updateItem(int itemId, int color) {
        dataModalArrayList.set(itemId, color);
        notifyDataSetChanged();
    }
}
