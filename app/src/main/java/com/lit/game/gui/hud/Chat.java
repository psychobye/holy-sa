package com.lit.game.gui.hud;

import static com.lit.game.core.Samp.activity;

import android.content.Context;
import android.text.Spanned;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.lit.game.R;
import com.lit.game.gui.util.FadingEdgeLayout;
import com.lit.game.gui.util.Utils;
import com.lit.launcher.storage.Storage;

import org.jetbrains.annotations.NotNull;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

public class Chat {
    native void SendChatButton(int buttonID);
    static native void SendChatMessage(byte str[]);
    native void toggleNativeKeyboard(boolean toggle);
    native void nativeToggleInputState(boolean toggle);
    native void clickHistoryButt(int buttId);

    static EditText chat_input;
    ConstraintLayout chat_input_layout;

    TextView me_button;
    TextView try_button;
    TextView do_button;
    ImageView hide_chat;
    ConstraintLayout chat_box;
    ConstraintLayout chat_binder_butt;

    private final int INVALID = -1;
    private final int ME_BUTTON = 0;
    private final int DO_BUTTON = 1;
    private final int TRY_BUTTON = 2;
    private int chat_button = INVALID;

    private int chatFontSize;

    FadingEdgeLayout chatBox;
    private RecyclerView chat;
    int defaultChatHeight;
    int defaultChatFontSize;

    HudManager.ChatAdapter adapter;
    ArrayList<Spanned> chat_lines = new ArrayList<>();

    public Chat() {

        chat_box = activity.findViewById(R.id.chat_box);

        /*chat_binder_butt = activity.findViewById(R.id.chat_binder_butt);
        chat_binder_butt.setOnClickListener(view -> {
            new Binder();
            toggleKeyboard(false);
        });

        hide_chat = activity.findViewById(R.id.hide_chat);
        hide_chat.setOnClickListener(view -> {
            if(chat_box.getVisibility() == View.GONE){
                showChat();
            }
            else{
                hideChat();
            }
        });

        me_button = activity.findViewById(R.id.me_button);
        me_button.setOnClickListener(view -> {
            if(chat_button == ME_BUTTON){
                me_button.setBackgroundTintList(null);
                chat_button = INVALID;
            }else {
                chat_button = ME_BUTTON;
                me_button.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#9c27b0")));
                try_button.setBackgroundTintList(null);
                do_button.setBackgroundTintList(null);
            }
            SendChatButton(chat_button);
        });

        try_button = activity.findViewById(R.id.try_button);
        try_button.setOnClickListener(view -> {
            if(chat_button == TRY_BUTTON){
                try_button.setBackgroundTintList(null);
                chat_button = INVALID;
            }else {
                chat_button = TRY_BUTTON;
                try_button.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#087f23")));
                me_button.setBackgroundTintList(null);
                do_button.setBackgroundTintList(null);
            }
            SendChatButton(chat_button);
        });

        do_button = activity.findViewById(R.id.do_button);
        do_button.setOnClickListener(view -> {
            if(chat_button == DO_BUTTON){
                do_button.setBackgroundTintList(null);
                chat_button = INVALID;
            }else {
                chat_button = DO_BUTTON;
                do_button.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#c67100")));
                try_button.setBackgroundTintList(null);
                me_button.setBackgroundTintList(null);
            }
            SendChatButton(chat_button);
        });*/

        chat_input_layout = activity.findViewById(R.id.chat_input_layout);
        chat_input_layout.setVisibility(View.GONE);
        chat_input = activity.findViewById(R.id.chat_input);
        chat_input.setShowSoftInputOnFocus(false);

        chat_input.setOnEditorActionListener(new EditText.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_SEND) {
                    try {
                        SendChatMessage(chat_input.getText().toString().getBytes("windows-1251"));
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    }

                    toggleKeyboard(false);
                   // ClickChatj();
                    return true;
                }
                return false;
            }
        });

        defaultChatFontSize = 27;
        chat = activity.findViewById(R.id.chat);

        FadingEdgeLayout chatBox = activity.findViewById(R.id.chat_fade_box);
        
        Storage.setInt("defaultChatHeight", chatBox.getMinimumHeight());

        int height = Storage.getInt("chatHeight");
        if(height > 100) {

            ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) chatBox.getLayoutParams();
            layoutParams.height = height;
            chatBox.setLayoutParams(layoutParams);
        }

        LinearLayoutManager mLayoutManager = new LinearLayoutManager(activity);

        mLayoutManager.setStackFromEnd(true);
        chat.setLayoutManager(mLayoutManager);


        // создаем адаптер
        adapter = new ChatAdapter(activity, chat_lines);
        // устанавливаем для списка адаптер
        chat.setAdapter(adapter);

        /*ConstraintLayout chat_up_butt = activity.findViewById(R.id.chat_up_butt);
        chat_up_butt.setOnClickListener(view -> {
            clickHistoryButt(1);
        });

        ConstraintLayout chat_down_butt = activity.findViewById(R.id.chat_down_butt);
        chat_down_butt.setOnClickListener(view -> {
            clickHistoryButt(0);
        });*/
    }

    void hideChat() {
        activity.runOnUiThread(() -> {
            chat_box.setVisibility(View.GONE);
            hide_chat.setRotation(180);
        });
    }

    void showChat() {
        activity.runOnUiThread(() -> {
            chat_box.setVisibility(View.VISIBLE);
            hide_chat.setRotation(0);
        });
    }

    public void ToggleChat(boolean toggle){
        activity.runOnUiThread(()-> {
            if(toggle){
                chat.setVisibility(View.VISIBLE);
            } else {
                chat.setVisibility(View.GONE);
            }
        });
    }
    public void AddChatMessage(String msg){
        adapter.addItem(msg);
    }

    public void ChangeChatFontSize(int size)
    {
        activity.runOnUiThread(() -> {
            if(size == -1){
                chatFontSize = defaultChatFontSize;
            }else{
                chatFontSize = size;
            }
            adapter = new ChatAdapter(activity, adapter.getItems());
            // устанавливаем для списка адаптер
            chat.setAdapter(adapter);
        });
    }


    public void AddToChatInput(String msg){
        activity.runOnUiThread(() -> {
            chat_input.setText(msg);
            int len = chat_input.getText().length();
            if(len >= 0) chat_input.setSelection(len);
        });

    }

    public void ToggleChatInput(boolean toggle){
        activity.runOnUiThread(() ->
        {
            if(toggle){
                chat_input_layout.setVisibility(View.VISIBLE);
            }else {
                chat_input_layout.setVisibility(View.GONE);
                chat_input.getText().clear();
            }
        });
    }

    void toggleKeyboard(boolean toggle) {
        ToggleChatInput(toggle);

        if(Storage.getBoolean("isAndroidKeyboard")) {
            nativeToggleInputState(toggle);
            // android клава
            chat_input.requestFocus();

            InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);

            if(toggle)
                imm.showSoftInput(chat_input, InputMethodManager.SHOW_IMPLICIT);
            else
                imm.hideSoftInputFromWindow(chat_input.getWindowToken(), 0);
        }
        else {
            // нативная
            toggleNativeKeyboard(toggle);
        }
    }
    public void ClickChatj(){
        activity.runOnUiThread(() -> {
            if (chat_input_layout.getVisibility() == View.VISIBLE) {
                toggleKeyboard(false);

            } else {
                toggleKeyboard(true);
            }
        });
    }

    public class ChatAdapter  extends RecyclerView.Adapter<ChatAdapter.ViewHolder>{

        private final LayoutInflater inflater;
        private final List<Spanned> chat_lines;

        ChatAdapter(Context context, List<Spanned> chat_lines) {
            this.chat_lines = chat_lines;
            this.inflater = LayoutInflater.from(context);
        }
        @NotNull
        @Override
        public ChatAdapter.ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {

            View view = inflater.inflate(R.layout.chatline, parent, false);
            view.setOnClickListener(view1 -> {
                //ClickChat();
                ClickChatj();
            });
            return new ViewHolder(view);
        }

        @Override
        public void onBindViewHolder(ChatAdapter.ViewHolder holder, int position) {

            holder.chat_line_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, chatFontSize);
            holder.chat_line_text.setText(chat_lines.get(position));
            //holder.chat_line_shadow.setText(Html.fromHtml(chat_lines.get(position)));

          //  holder.chat_line_shadow.setTextSize(TypedValue.COMPLEX_UNIT_PX, chatFontSize);
        }

        @Override
        public int getItemCount() {
            return chat_lines.size();
        }

        public List getItems() {
            return chat_lines;
        }

        public class ViewHolder extends RecyclerView.ViewHolder {
            final TextView chat_line_text;
         //   final TextView chat_line_shadow;
            ViewHolder(View view){
                super(view);
                chat_line_text = view.findViewById(R.id.chat_line_text);
             //   chat_line_shadow = view.findViewById(R.id.chat_line_shadow);
            }
        }
        public void addItem(String item) {
            activity.runOnUiThread(() -> {
                if(this.chat_lines.size() > 40){
                    this.chat_lines.remove(0);
                    notifyItemRemoved(0);
                }
                //this.chat_lines.add(" "+item+" ");

                this.chat_lines.add(Utils.transfromColors(item));
                notifyItemInserted(this.chat_lines.size()-1);

                if(chat.getScrollState() == RecyclerView.SCROLL_STATE_IDLE) {
                    chat.scrollToPosition(this.chat_lines.size()-1);
                }
            });

        }
    }

}
