package com.holy.game.gui.dialogs;

import android.os.Handler;
import android.os.Looper;
import android.text.Layout;
import android.text.TextPaint;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.lit.game.R;
import com.lit.game.gui.util.Utils;

import java.util.ArrayList;

public class DialogAdapter extends RecyclerView.Adapter<DialogAdapter.ViewHolder> {
    private int mCurrentSelectedPosition = 0;
    private final ArrayList<TextView> mFieldHeaders;
    private final ArrayList<String> mFieldTexts;
    private OnClickListener mOnClickListener;
    private OnDoubleClickListener mOnDoubleClickListener;

    private volatile int[] mColumnWidths = null;
    private volatile boolean mComputingWidths = false;

    public interface OnClickListener { void onClick(int i, String str); }
    public interface OnDoubleClickListener { void onDoubleClick(); }

    public DialogAdapter(ArrayList<String> fields, ArrayList<TextView> fieldHeaders) {
        this.mFieldTexts = fields != null ? fields : new ArrayList<>();
        this.mFieldHeaders = fieldHeaders != null ? fieldHeaders : new ArrayList<>();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.sd_dialog_item, parent, false);
        return new ViewHolder(v);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        if (position < 0 || position >= mFieldTexts.size()) return;

        String raw = mFieldTexts.get(position);
        String[] cols = raw.split("\t");

        if (mColumnWidths == null && !mComputingWidths) {
            startComputeWidthsAsync(holder);
        }

        int fieldCount = holder.mFields.size();
        for (int i = 0; i < fieldCount; i++) {
            TextView tv = holder.mFields.get(i);
            if (i < cols.length) {
                CharSequence tx = Utils.transfromColors(cols[i].replace("\\t", ""));
                tv.setText(tx);
                tv.setVisibility(View.VISIBLE);
            } else {
                tv.setText("");
                tv.setVisibility(View.GONE);
            }

            if (mColumnWidths != null && i < mColumnWidths.length) {
                tv.setMinWidth(mColumnWidths[i]);
            }
        }

        if (mColumnWidths != null) {
            for (int i = 0; i < mFieldHeaders.size() && i < mColumnWidths.length; i++) {
                TextView h = mFieldHeaders.get(i);
                if (h != null) h.setMinWidth(mColumnWidths[i]);
            }
        }

        if (mCurrentSelectedPosition == position) {
            holder.mFieldBg.setVisibility(View.VISIBLE);
            holder.mFieldBg.setImageResource(R.drawable.dialog_item_btn);
        } else {
            holder.mFieldBg.setVisibility(View.VISIBLE);
            holder.mFieldBg.setImageResource(R.drawable.dialog_item_btn_none);
        }

        holder.getView().setOnClickListener(view -> {
            int adapterPos = holder.getAdapterPosition();
            if (adapterPos == RecyclerView.NO_POSITION) return;

            if (mCurrentSelectedPosition != adapterPos) {
                int old = mCurrentSelectedPosition;
                mCurrentSelectedPosition = adapterPos;
                notifyItemChanged(old, 0);
                notifyItemChanged(mCurrentSelectedPosition, 0);
                if (mOnClickListener != null) {
                    String first = "";
                    if (cols != null && cols.length > 0) first = cols[0].replace("\\t", "");
                    mOnClickListener.onClick(adapterPos, first);
                }
                return;
            }

            if (mOnDoubleClickListener != null) mOnDoubleClickListener.onDoubleClick();
        });
    }

    public void updateSizes() {
        mColumnWidths = null;
        if (Looper.myLooper() == Looper.getMainLooper()) {
            notifyDataSetChanged();
        } else {
            new Handler(Looper.getMainLooper()).post(this::notifyDataSetChanged);
        }
    }

    @Override
    public int getItemCount() { return this.mFieldTexts.size(); }

    public void setOnClickListener(OnClickListener onClickListener) { this.mOnClickListener = onClickListener; }
    public void setOnDoubleClickListener(OnDoubleClickListener onDoubleClickListener) { this.mOnDoubleClickListener = onDoubleClickListener; }

    private void startComputeWidthsAsync(ViewHolder holderForPaint) {
        if (mComputingWidths) return;
        mComputingWidths = true;

        final TextPaint samplePaint = holderForPaint.getAnyFieldPaint();
        final int columns = Math.max(1, Math.max(getMaxColumnsFromData(), mFieldHeaders.size()));

        new Thread(() -> {
            int[] max = new int[columns];

            int defaultPadding = (int) TypedValue.applyDimension(
                    TypedValue.COMPLEX_UNIT_DIP, 8, holderForPaint.getView().getResources().getDisplayMetrics());
            int fieldPadding = defaultPadding;
            if (!holderForPaint.mFields.isEmpty()) {
                TextView tv0 = holderForPaint.mFields.get(0);
                fieldPadding = tv0.getPaddingLeft() + tv0.getPaddingRight();
            } else if (!mFieldHeaders.isEmpty()) {
                TextView h0 = mFieldHeaders.get(0);
                fieldPadding = h0.getPaddingLeft() + h0.getPaddingRight();
            }

            for (String raw : mFieldTexts) {
                if (TextUtils.isEmpty(raw)) continue;
                String[] parts = raw.split("\t");
                for (int i = 0; i < parts.length && i < columns; i++) {
                    CharSequence cs = Utils.transfromColors(parts[i].replace("\\t", ""));
                    float w = Layout.getDesiredWidth(cs, samplePaint);
                    int wi = (int) Math.ceil(w) + fieldPadding + 4;
                    if (max[i] < wi) max[i] = wi;
                }
            }

            for (int i = 0; i < mFieldHeaders.size() && i < columns; i++) {
                TextView h = mFieldHeaders.get(i);
                if (h == null) continue;
                CharSequence txt = h.getText();
                float w = Layout.getDesiredWidth(txt, h.getPaint());
                int wi = (int) Math.ceil(w) + h.getPaddingLeft() + h.getPaddingRight() + 4;
                if (max[i] < wi) max[i] = wi;
            }

            for (int i = 0; i < columns; i++) {
                if (max[i] < 48) max[i] = 48;
            }

            final int[] computed = max;
            new Handler(Looper.getMainLooper()).post(() -> {
                mColumnWidths = computed;
                mComputingWidths = false;
                notifyDataSetChanged();
            });
        }).start();
    }

    private int getMaxColumnsFromData() {
        int max = 0;
        for (String s : mFieldTexts) {
            if (s == null) continue;
            int cols = s.split("\t", -1).length;
            if (cols > max) max = cols;
        }
        return Math.max(1, max);
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public ImageView mFieldBg;
        public ArrayList<TextView> mFields = new ArrayList<>();
        private final View mView;

        public ViewHolder(View itemView) {
            super(itemView);
            this.mView = itemView;
            this.mFieldBg = itemView.findViewById(R.id.sd_dialog_item_bg);
            ConstraintLayout field = itemView.findViewById(R.id.sd_dialog_item_main);
            for (int i = 1; i < field.getChildCount(); i++) {
                View child = field.getChildAt(i);
                if (child instanceof TextView) this.mFields.add((TextView) child);
            }
        }

        public View getView() { return this.mView; }

        public TextPaint getAnyFieldPaint() {
            if (!mFields.isEmpty()) return mFields.get(0).getPaint();
            TextView tv = new TextView(itemView.getContext());
            return tv.getPaint();
        }
    }
}