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
	private static final String PANELOUTPUT_600 = "PANELOUTPUT_600";
	private static final String PANELOUTPUT_600PORT = "PANELOUTPUT_600PORT";
	private static final String PANELOUTPUT_768 = "PANELOUTPUT_768";
	private static final String PANELOUTPUT_768PORT = "PANELOUTPUT_768PORT";
	private static final String PANELOUTPUT_720 = "PANELOUTPUT_720";
	private static final String PANELOUTPUT_720PORT = "PANELOUTPUT_720PORT";
	private static final String PANELOUTPUT_1080 = "PANELOUTPUT_1080";
	private static final String PANELOUTPUT_1080PORT = "PANELOUTPUT_1080PORT";
	private static final String PANELOUTPUT_800PORT1 = "PANELOUTPUT_800PORT1";
	private static final String PANELOUTPUT_800PORT2 = "PANELOUTPUT_800PORT2";
	private static final String PANELOUTPUT_800PORT3 = "PANELOUTPUT_800PORT3";
	private static final String PANELOUTPUT_800PORT4 = "PANELOUTPUT_800PORT4";
	
    private static final int OUTPUT_600 = 0;
	private static final int OUTPUT_600PORT = 1;
	private static final int OUTPUT_768 = 2;
	private static final int OUTPUT_768PORT = 3;
	private static final int OUTPUT_720 = 4;
	private static final int OUTPUT_720PORT = 5;
	private static final int OUTPUT_1080 = 6;
	private static final int OUTPUT_1080PORT = 7;
	private static final int OUTPUT_800PORT1 = 8;
	private static final int OUTPUT_800PORT2 = 9;
	private static final int OUTPUT_800PORT3 = 10;
	private static final int OUTPUT_800PORT4 = 11;
    
	private Context mContext;
	private static final int PANELOUTPUT_SET_DELAY_MS = 500;
	private final Handler mDelayHandler = new Handler();
	private SystemControlManager mSystemControl = SystemControlManager.getInstance();
	private String mNewKeyDefinition;

	private final Runnable mPanelOutputRunnable = new Runnable() {
		@Override
		public void run() {			
			if (PANELOUTPUT_600.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_600);
			}else if (PANELOUTPUT_600PORT.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_600PORT);
			}else if (PANELOUTPUT_768.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_768);
			}else if (PANELOUTPUT_768PORT.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_768PORT);
			}else if (PANELOUTPUT_720.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_720);
			} else if (PANELOUTPUT_720PORT.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_720PORT);
			}else if (PANELOUTPUT_1080.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_1080);
			}else if (PANELOUTPUT_1080PORT.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_1080PORT);
			}else if (PANELOUTPUT_800PORT1.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_800PORT1);
			}else if (PANELOUTPUT_800PORT2.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_800PORT2);
			}else if (PANELOUTPUT_800PORT3.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_800PORT3);
			}else if (PANELOUTPUT_800PORT4.equals(mNewKeyDefinition)) {
				setPanelOutput(OUTPUT_800PORT4);
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
		actions.add(new Action.Builder().key(PANELOUTPUT_600).title(getString(R.string.panel600))
				.checked(checkedKey == OUTPUT_600).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_600PORT).title(getString(R.string.panel600port))
				.checked(checkedKey == OUTPUT_600PORT).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_768).title(getString(R.string.panel768))
				.checked(checkedKey == OUTPUT_768).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_768PORT).title(getString(R.string.panel768port))
				.checked(checkedKey == OUTPUT_768PORT).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_720).title(getString(R.string.panel720))
				.checked(checkedKey == OUTPUT_720).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_720PORT).title(getString(R.string.panel720port))
				.checked(checkedKey == OUTPUT_720PORT).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_1080).title(getString(R.string.panel1080))
				.checked(checkedKey == OUTPUT_1080).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_1080PORT).title(getString(R.string.panel1080port))
				.checked(checkedKey == OUTPUT_1080PORT).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_800PORT1).title(getString(R.string.panel800port1))
				.checked(checkedKey == OUTPUT_800PORT1).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_800PORT2).title(getString(R.string.panel800port2))
				.checked(checkedKey == OUTPUT_800PORT2).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_800PORT3).title(getString(R.string.panel800port3))
				.checked(checkedKey == OUTPUT_800PORT3).build());
		actions.add(new Action.Builder().key(PANELOUTPUT_800PORT4).title(getString(R.string.panel800port4))
				.checked(checkedKey == OUTPUT_800PORT4).build());
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
		
		String defaultPanelOutput;
		if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
		{
			defaultPanelOutput = "800x1280_1";
		}
		else
		{
			defaultPanelOutput = "1024x600";
		}
		
		if("1024x600".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_600;
		}
		else if("600x1024".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_600PORT;
		}
		else if("1024x768".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_768;
		}
		else if("768x1024".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_768PORT;
		}
		else if("1280x720".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_720;
		}
		else if("720x1280".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_720PORT;
		}
		else if("1920x1080".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_1080;
		}
		else if("1080x1920".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_1080PORT;
		}
		else if("800x1280_1".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_800PORT1;
		}
		else if("800x1280_2".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_800PORT2;
		}
		else if("800x1280_3".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_800PORT3;
		}
		else if("800x1280_4".equals(mSystemControl.getPropertyString("persist.sys.builtin.ui_mode", defaultPanelOutput)))
		{
			return OUTPUT_800PORT4;
		}		
		else
		{
			return OUTPUT_600;
		}
	}

	private void setPanelOutput(int keyValue) {
		switch(keyValue){
			case OUTPUT_600:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "1024x600");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_7");
				}
				else
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_1");
				}
				break;
			}
			case OUTPUT_600PORT:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "600x1024");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_1");
				}
				else
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_3");
				}
				break;
			}
			case OUTPUT_768:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "1024x768");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					
				}
				else
				{
					//mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_4");
				}
				break;
			}
			case OUTPUT_768PORT:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "768x1024");				
				mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_0");				
				break;
			}
			case OUTPUT_720:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "1280x720");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_2");
				}
				else
				{
					//mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_5");
				}
				break;
			}
			case OUTPUT_720PORT:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "720x1280");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_6");
				}
				else
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_2");
				}
				break;
			}
			case OUTPUT_1080:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "1920x1080");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_3");
				}
				else
				{
					//mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_6");
				}
				break;
			}
			case OUTPUT_1080PORT:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "1080x1920");
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_7");
				}
				break;
			}
			case OUTPUT_800PORT1:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "800x1280_1");				
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_4");
				}
				break;
			}
			case OUTPUT_800PORT2:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "800x1280_2");				
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_5");
				}
				break;
			}
			case OUTPUT_800PORT3:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "800x1280_3");				
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_6");
				}
				break;
			}
			case OUTPUT_800PORT4:{
				mSystemControl.setProperty("persist.sys.builtin.ui_mode", "800x1280_4");				
				if("u202".equals(mSystemControl.getPropertyString("ro.product.board", "u202")))
				{
					mSystemControl.setBootenv("ubootenv.var.panel_type","lcd_9");
				}
				break;
			}
			default:
                break;
		}
	}
}
