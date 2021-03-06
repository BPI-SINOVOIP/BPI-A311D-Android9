/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.tv.droidlogic.channelui;

import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import android.util.Log;

import com.android.tv.ui.sidepanel.Item;
import com.android.tv.R;

public abstract class SeekBarItem extends Item {
    private final String mCheckedTitle;
    private final String mUncheckedTitle;
    private final int mDescription;
    private TextView mTextView;
    private SeekBar mSeekBar;

    public SeekBarItem(String title, int description) {
        this(title, title, description);
    }

    public SeekBarItem(String checkedTitle, String uncheckedTitle, int description) {
        mCheckedTitle = checkedTitle;
        mUncheckedTitle = uncheckedTitle;
        mDescription = description;
    }

    @Override
    protected int getResourceId() {
        return R.layout.option_item_seekbar;
    }

    protected int getSeekBarId() {
        return R.id.seekbar;
    }

    protected int getTitleViewId() {
        return R.id.title;
    }

    protected int getDescriptionViewId() {
        return R.id.description;
    }

    @Override
    protected void onBind(View view) {
        super.onBind(view);
        mSeekBar = (SeekBar) view.findViewById(getSeekBarId());
        mTextView = (TextView) view.findViewById(getTitleViewId());
        TextView descriptionView = (TextView) view.findViewById(getDescriptionViewId());
        mTextView.setText(mDescription * 5 + "%");
        mSeekBar.setProgress(mDescription + 20);
        /*
        if (mDescription != null) {
            descriptionView.setVisibility(View.VISIBLE);
            descriptionView.setText(mDescription);
        } else {
            descriptionView.setVisibility(View.GONE);
        }*/
        descriptionView.setVisibility(View.GONE);
    }

    @Override
    protected void onUnbind() {
        super.onUnbind();
        mTextView = null;
        mSeekBar = null;
    }

    @Override
    protected void onUpdate() {
        super.onUpdate();
        if (mTextView != null) {
            mTextView.setText(mDescription * 5 + "%");
        }
        //updateInternal();
    }
    /*
    public void setChecked(boolean checked) {
        if (mChecked != checked) {
            mChecked = checked;
            updateInternal();
        }
    }

    public boolean isChecked() {
        return mChecked;
    }

    private void updateInternal() {
        if (isBound()) {
            mTextView.setText(mChecked ? mCheckedTitle : mUncheckedTitle);
            mCompoundButton.setChecked(mChecked);
        }
    }*/
    @Override
    protected void onSelected() {
        //setChecked(true);
    }

    @Override
    protected void onProgressChanged(SeekBar seekBar, int progress) {
        mTextView.setText(progress * 5 + "%");
    }
}
