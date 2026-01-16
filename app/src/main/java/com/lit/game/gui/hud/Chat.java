package com.lit.game.gui.hud;

import static com.lit.game.core.Samp.activity;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.text.Spanned;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.DecelerateInterpolator;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.lit.game.R;
import com.lit.game.gui.util.Utils;
import com.lit.launcher.storage.Storage;

import org.jetbrains.annotations.NotNull;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

public class Chat {
    public static Chat instance;
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

    private RecyclerView chat;
    private View chatBgOverlay;
    int defaultChatHeight;
    int defaultChatFontSize;

    HudManager.ChatAdapter adapter;
    ArrayList<Spanned> chat_lines = new ArrayList<>();

    private boolean overlayVisible = false;
    private boolean inputShown = false;

    private final Handler autoHideHandler = new Handler(Looper.getMainLooper());
    private final long AUTO_HIDE_DELAY = 10000;
    private boolean isChatHiddenByTimer = false;

    private final Runnable autoHideRunnable = () -> {
        boolean isEnabled = true;

        if (isEnabled && !inputShown) {
            activity.runOnUiThread(() -> {
                if (chat != null) {
                    isChatHiddenByTimer = true;
                    chat.animate().alpha(0f).setDuration(500).start();
                    if (chatBgOverlay != null) {
                        chatBgOverlay.animate().alpha(0f).setDuration(500).start();
                    }
                }
            });
        }
    };

    public Chat() {
        instance = this;

        chat_box = activity.findViewById(R.id.chat_box);

        chat_input_layout = activity.findViewById(R.id.chat_input_layout);
        chat_input_layout.setVisibility(View.GONE);
        chat_input = activity.findViewById(R.id.chat_input);
        if (chat_input != null) chat_input.setShowSoftInputOnFocus(false);

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
                    return true;
                }
                return false;
            }
        });

        defaultChatFontSize = 27;
        chat = activity.findViewById(R.id.chat);
        chatBgOverlay = activity.findViewById(R.id.chat_bg_overlay);

        if (chatBgOverlay != null) {
            chatBgOverlay.post(() -> {
                Storage.setInt("defaultChatHeight", chatBgOverlay.getHeight());
            });
        }

        int height = Storage.getInt("chatHeight");
        if (height > 100) {
            if (chatBgOverlay != null) {
                ViewGroup.LayoutParams lp = chatBgOverlay.getLayoutParams();
                if (lp != null) {
                    lp.height = height;
                    chatBgOverlay.setLayoutParams(lp);
                }
            }
            if (chat != null) {
                ViewGroup.LayoutParams lp2 = chat.getLayoutParams();
                if (lp2 != null) {
                    lp2.height = height;
                    chat.setLayoutParams(lp2);
                }
            }
        }

        if (chat != null) {
            chat.setClipToPadding(false);
            LinearLayoutManager mLayoutManager = new LinearLayoutManager(activity);
            mLayoutManager.setStackFromEnd(true);
            chat.setOverScrollMode(View.OVER_SCROLL_ALWAYS);
            chat.setLayoutManager(mLayoutManager);

            chat.addOnScrollListener(new RecyclerView.OnScrollListener() {
                @Override
                public void onScrolled(@NotNull RecyclerView recyclerView, int dx, int dy) {
                    super.onScrolled(recyclerView, dx, dy);
                
                    resetAutoHideTimer();

                    if (isAtBottom()) {
                        if (!inputShown) hideOverlay();
                    } else {
                        showOverlay();
                    }
                }
            });

            chat.setOnTouchListener((v, event) -> {
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    if (isChatHiddenByTimer) {
                        resetAutoHideTimer();
                        return true;
                    }
                    resetAutoHideTimer();
                }
                return false;
            });
        }

        adapter = new ChatAdapter(activity, chat_lines);
        if (chat != null) chat.setAdapter(adapter);

        if (chatBgOverlay != null) {
            chatBgOverlay.setAlpha(0f);
            chatBgOverlay.setVisibility(View.GONE);
            overlayVisible = false;
        }

        resetAutoHideTimer();
    }

    private void resetAutoHideTimer() {
        activity.runOnUiThread(() -> {
            autoHideHandler.removeCallbacks(autoHideRunnable);

            if (chat != null && chat.getAlpha() < 1f) {
                chat.animate().cancel();
                chat.setAlpha(1f);
                isChatHiddenByTimer = false;

                if (!isAtBottom() || inputShown) {
                    showOverlay();
                }
            }

            if (!inputShown) {
                autoHideHandler.postDelayed(autoHideRunnable, AUTO_HIDE_DELAY);
            }
        });
    }

    private void showOverlay() {
        if (chatBgOverlay == null) return;
        activity.runOnUiThread(() -> {
            if (overlayVisible) {
                chatBgOverlay.animate().alpha(1f).setDuration(150).start();
                return;
            }
            chatBgOverlay.animate().cancel();
            chatBgOverlay.setVisibility(View.VISIBLE);
            chatBgOverlay.setAlpha(0f);
            chatBgOverlay.animate()
                    .alpha(1f)
                    .setDuration(180)
                    .setInterpolator(new DecelerateInterpolator())
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            overlayVisible = true;
                        }
                    }).start();
        });
    }

    private void hideOverlay() {
        if (chatBgOverlay == null) return;
        activity.runOnUiThread(() -> {
            if (!overlayVisible) {
                chatBgOverlay.setAlpha(0f);
                chatBgOverlay.setVisibility(View.GONE);
                overlayVisible = false;
                return;
            }
            chatBgOverlay.animate().cancel();
            chatBgOverlay.animate()
                    .alpha(0f)
                    .setDuration(180)
                    .setInterpolator(new DecelerateInterpolator())
                    .setListener(new AnimatorListenerAdapter() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            chatBgOverlay.setVisibility(View.GONE);
                            overlayVisible = false;
                        }
                    }).start();
        });
    }

    private boolean isAtBottom() {
        if (chat == null || chat.getLayoutManager() == null) return true;
        RecyclerView.LayoutManager lm = chat.getLayoutManager();
        if (!(lm instanceof LinearLayoutManager)) return true;
        LinearLayoutManager llm = (LinearLayoutManager) lm;
        int lastVisible = llm.findLastCompletelyVisibleItemPosition();
        int total = adapter != null ? adapter.getItemCount() : 0;
        return total == 0 || lastVisible >= total - 1;
    }

    private boolean isInputShown() {
        return chat_input_layout != null && chat_input_layout.getVisibility() == View.VISIBLE;
    }

    // ---------------- API ----------------

    public static void hideChat() {
        activity.runOnUiThread(() -> {
            View box = activity.findViewById(R.id.chat_box);
            if (box != null) box.setVisibility(View.GONE);
        });
    }

    public static void showChat() {
        activity.runOnUiThread(() -> {
            final View overlay = activity.findViewById(R.id.chat_bg_overlay);
            final View chatView = activity.findViewById(R.id.chat);
            final View box = activity.findViewById(R.id.chat_box);

            if (box != null) box.setVisibility(View.VISIBLE);

            if (overlay == null) {
                if (chatView != null) chatView.setVisibility(View.VISIBLE);
                return;
            }

            if (chatView != null) {
                chatView.setVisibility(View.VISIBLE);
                chatView.animate().cancel();
                chatView.setAlpha(1f);
            }

            overlay.setVisibility(View.VISIBLE);
            overlay.animate().cancel();
            overlay.setAlpha(1f);

            if (chatView != null) chatView.setVisibility(View.VISIBLE);

            overlay.postDelayed(() -> {
                overlay.animate().cancel();
                overlay.animate()
                        .alpha(0f)
                        .setDuration(300)
                        .setInterpolator(new DecelerateInterpolator())
                        .withEndAction(() -> {
                            overlay.setVisibility(View.GONE);
                            overlay.setAlpha(0f);
                        })
                        .start();
            }, 3000);

            if (instance != null) {
                instance.resetAutoHideTimer();
            }
        });
    }

    public void ToggleChat(boolean toggle){
        activity.runOnUiThread(()-> {
            if(toggle){
                if (chat != null) chat.setVisibility(View.VISIBLE);
                resetAutoHideTimer();
            } else {
                if (chat != null) chat.setVisibility(View.GONE);
                autoHideHandler.removeCallbacks(autoHideRunnable);
            }
        });
    }

    public void AddChatMessage(String msg){
        adapter.addItem(msg);
        resetAutoHideTimer();
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
            if (chat != null) chat.setAdapter(adapter);
        });
    }


    public void AddToChatInput(String msg){
        activity.runOnUiThread(() -> {
            if (chat_input != null) {
                chat_input.setText(msg);
                int len = chat_input.getText().length();
                if(len >= 0) chat_input.setSelection(len);
            }
        });

    }

    public void ToggleChatInput(boolean toggle){
        activity.runOnUiThread(() -> {
            if (chat_input_layout == null) return;

            inputShown = toggle;

            if(toggle){
                chat_input_layout.setVisibility(View.VISIBLE);
                showOverlay();
                autoHideHandler.removeCallbacks(autoHideRunnable);
                if (chat != null) chat.setAlpha(1f);
            } else {
                chat_input_layout.setVisibility(View.GONE);
                if (isAtBottom()) hideOverlay();
                if (chat_input != null) chat_input.getText().clear();

                resetAutoHideTimer();
            }
        });
    }

    void toggleKeyboard(boolean toggle) {
        ToggleChatInput(toggle);

        if(Storage.getBoolean("isAndroidKeyboard")) {
            nativeToggleInputState(toggle);
            if (chat_input != null) chat_input.requestFocus();

            InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);

            if(toggle)
                imm.showSoftInput(chat_input, InputMethodManager.SHOW_IMPLICIT);
            else
                imm.hideSoftInputFromWindow(chat_input.getWindowToken(), 0);
        }
        else {
            toggleNativeKeyboard(toggle);
        }
    }

    public void ClickChatj(){
        activity.runOnUiThread(() -> {
            if (isChatHiddenByTimer) {
                resetAutoHideTimer();
                return;
            }

            if (chat_input_layout.getVisibility() == View.VISIBLE) {
                toggleKeyboard(false);
            } else {
                toggleKeyboard(true);
            }
            resetAutoHideTimer();
        });
    }

    // ---------------- Adapter ----------------
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
                ClickChatj();
            });
            return new ViewHolder(view);
        }

        @Override
        public void onBindViewHolder(ChatAdapter.ViewHolder holder, int position) {

            holder.chat_line_text.setTextSize(TypedValue.COMPLEX_UNIT_PX, chatFontSize);
            holder.chat_line_text.setText(chat_lines.get(position));
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
            ViewHolder(View view){
                super(view);
                chat_line_text = view.findViewById(R.id.chat_line_text);
            }
        }
        public void addItem(String item) {
            activity.runOnUiThread(() -> {
                if(this.chat_lines.size() > 40){
                    this.chat_lines.remove(0);
                    notifyItemRemoved(0);
                }

                this.chat_lines.add(Utils.transfromColors(item));
                notifyItemInserted(this.chat_lines.size()-1);

                if(chat.getScrollState() == RecyclerView.SCROLL_STATE_IDLE) {
                    chat.scrollToPosition(this.chat_lines.size()-1);
                }

                if (!isAtBottom()) showOverlay();
                else if (!inputShown) hideOverlay();
            });

        }
    }
}