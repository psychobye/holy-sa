package com.lit.game.gui.tab;

import android.os.Build;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.RequiresApi;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.lit.game.R;
import com.lit.game.core.Samp;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;


public class TabAdapter extends RecyclerView.Adapter {

    List<PlayerData> activelist = new ArrayList<>();
    List<PlayerData> savedlist = new ArrayList<>();

    @RequiresApi(api = Build.VERSION_CODES.N)
    public void updateSearch(String str) {
        if(str.isEmpty())
            activelist = savedlist;
        else
            activelist = savedlist.stream()
                    .filter(employee -> employee.getName().toLowerCase().contains(str.toLowerCase()))
                    .collect(Collectors.toList()
                    );

        notifyDataSetChanged();
    }

    public void addItem(PlayerData item) {
        activelist.add(item);
        savedlist.add(item);
        notifyItemInserted(activelist.size()-1);
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new ViewHolder(LayoutInflater.from(parent.getContext()).inflate(R.layout.tab_item, parent, false));
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        onBindViewHolder((ViewHolder) holder, position);
    }

    public void onBindViewHolder(ViewHolder holder, int position) {
        PlayerData data = activelist.get(position);

        holder.name.setTextColor(data.color);

        holder.id.setText(String.valueOf(data.getId()));
        holder.name.setText(data.getName());
        holder.level.setText(String.valueOf(data.getLevel()));
        holder.ping.setText(String.valueOf(data.getPing()));
        holder.View.setOnClickListener(view ->  {
            view.startAnimation(Samp.getClickAnim());
        });
    }

    @Override
    public int getItemCount() {
        return activelist.size();
    }


    public static class ViewHolder extends RecyclerView.ViewHolder {
        public TextView id;
        public TextView level;
        public View mView;
        public ConstraintLayout View;
        public TextView name;
        public TextView ping;

        public ViewHolder(View itemView) {
            super(itemView);
            this.mView  = itemView;
            this.View   = itemView.findViewById(R.id.hassle_tab_item);
            this.id     = itemView.findViewById(R.id.player_id);
            this.name   = itemView.findViewById(R.id.player_name);
            this.level  = itemView.findViewById(R.id.player_level);
            this.ping   = itemView.findViewById(R.id.player_ping);
        }

        public View getView() {
            return this.mView;
        }
    }
}
