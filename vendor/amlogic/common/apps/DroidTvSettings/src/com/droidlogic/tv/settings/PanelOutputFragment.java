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
 * limitations under the License
 */

package com.droidlogic.tv.settings;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.os.PowerManager;
import android.provider.Settings;
import android.support.v17.preference.LeanbackPreferenceFragment;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceScreen;
import android.util.ArrayMap;
import android.util.Log;

import com.droidlogic.tv.settings.R;
import com.droidlogic.tv.settings.RadioPreference;
import com.droidlogic.tv.settings.dialog.old.Action;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import com.droidlogic.app.SystemControlManager;

public class PanelOutputFragment extends LeanbackPreferenceFragment {
	private static final String TAG = "PanelOutputFragment";
	private static final String PANELOUTPUT_RADIO_GROUP = "PanelOutput";
	private static final String PANELOUTPUT_800PORT = "PANELOUTPUT_800PORT";
	private static final String PANELOUTPUT_1200PORT = "PANELOUTPUT_1200PORT";
	
	private static final int OUTPUT_800PORT = 0;
	private static final int OUTPUT_1200PORT = 1;
    
	private Context mContext;
	private static final int PANELOUTPUT_SET_DELAY_MS = 500;
	private final Handler mDelayHandler = new Handler();
	private SystemControlManager mSystemControl = SystemControlManager.getInstance();
	private String mNewKeyDefinition;

	private final Runnable mPanelOutputRunnable = new Runnable() {
		@Override
		public void run() {			
			if (PANELOUTPUT_800PORT.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_800PORT);
			}else if (PANELOUTPUT_1200PORT.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_1200PORT);
			}
		}
	};

	public static PanelOutputFragment newInstance() {
		return new PanelOutputFragment();
	}

	@Override
	public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
		final Context themedContext = getPreferenceManager().getContext();
		mContext = themedContext;

		final PreferenceScreen screen = getPreferenceManager().createPreferenceScreen(themedContext);
		screen.setTitle(R.string.panel_output);
		String currentPanelOutput = null;
		Preference activePref = null;

		final List<Action> paneloutputList = getActions();
		for (final Action paneloutput : paneloutputList) {
			final String paneloutputTag = paneloutput.getKey();
			final RadioPreference radioPreference = new RadioPreference(themedContext);
			radioPreference.setKey(paneloutputTag);
			radioPreference.setPersistent(false);
			radioPreference.setTitle(paneloutput.getTitle());
			radioPreference.setRadioGroup(PANELOUTPUT_RADIO_GROUP);
			radioPreference.setLayoutResource(R.layout.preference_reversed_widget);
			if (paneloutput.isChecked()) {
				currentPanelOutput = paneloutputTag;
				radioPreference.setChecked(true);
				activePref = radioPreference;
			}
			screen.addPreference(radioPreference);
		}
		if (activePref != null && savedInstanceState == null) {
			scrollToPreference(activePref);
		}
		setPreferenceScreen(screen);
	}

	private ArrayList<Action> getActions() {
		ArrayList<Action> actions = new ArrayList<Action>();
		int checkedKey = whichPanelOutput();
		actions.add(new Action.Builder().key(PANELOUTPUT_800PORT).title(getString(R.string.panel800port))
				.checked(checkedKey == OUTPUT_800PORT).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_1200PORT).title(getString(R.string.panel1200port))
				.checked(checkedKey == OUTPUT_1200PORT).build());
		return actions;
	}

	@Override
	public boolean onPreferenceTreeClick(Preference preference) {
		if (preference instanceof RadioPreference) {
			final RadioPreference radioPreference = (RadioPreference) preference;
			radioPreference.clearOtherRadioPreferences(getPreferenceScreen());
			mNewKeyDefinition = radioPreference.getKey().toString();
			mDelayHandler.removeCallbacks(mPanelOutputRunnable);
			mDelayHandler.postDelayed(mPanelOutputRunnable, PANELOUTPUT_SET_DELAY_MS);
			radioPreference.setChecked(true);
		}
		return super.onPreferenceTreeClick(preference);
	}

	private int whichPanelOutput() {
		if("800x1280".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", "")))
		{
			return OUTPUT_800PORT;
		}
		else if("1200x1920".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", "")))
		{
			return OUTPUT_1200PORT;
		}
		else
		{
			return OUTPUT_800PORT;
		}
	}

	private void setPanelOutput(int keyValue) {
		switch(keyValue){
			case OUTPUT_800PORT:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "800x1280");
				mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_0");
				break;
			}
			case OUTPUT_1200PORT:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "1200x1920");				
				mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_1");				
				break;
			}
			default:
                		break;
		}
	}
}
