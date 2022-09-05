package com.android.tv.settings.bluetooth;

import android.app.AlertDialog;
import android.app.Notification;
import android.app.Service;
import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothHeadset;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import com.android.tv.settings.R;
import com.android.settingslib.bluetooth.BluetoothCallback;
import com.android.settingslib.bluetooth.CachedBluetoothDevice;
import com.android.settingslib.bluetooth.CachedBluetoothDeviceManager;
import com.android.settingslib.bluetooth.LocalBluetoothAdapter;
import com.android.settingslib.bluetooth.LocalBluetoothManager;
import com.android.settingslib.bluetooth.LocalBluetoothProfile;
import com.android.settingslib.bluetooth.LocalBluetoothProfileManager;
import com.android.settingslib.bluetooth.LocalBluetoothProfileManager.ServiceListener;

import java.util.Collection;
import java.util.List;
import java.util.Set;

public class DockService {
    public static class DockBluetoothCallback implements BluetoothCallback {
        private final Context mContext;

        public DockBluetoothCallback(Context context) {
            mContext = context;
        }

        public void onBluetoothStateChanged(int bluetoothState) {
        }

        public void onDeviceAdded(CachedBluetoothDevice cachedDevice) {
        }

        public void onDeviceDeleted(CachedBluetoothDevice cachedDevice) {
        }

        public void onConnectionStateChanged(CachedBluetoothDevice cachedDevice, int state) {
        }
        
        public void onAudioModeChanged(){
            //do nothing
        }
        
        public void onActiveDeviceChanged(CachedBluetoothDevice activeDevice, int bluetoothProfile){
            //do nothing
        }


        @Override
        public void onScanningStateChanged(boolean started) {
            // TODO: Find a more unified place for a persistent BluetoothCallback to live
            // as this is not exactly dock related.
            LocalBluetoothPreferences.persistDiscoveringTimestamp(mContext);
        }

        @Override
        public void onDeviceBondStateChanged(CachedBluetoothDevice cachedDevice, int bondState) {
            BluetoothDevice device = cachedDevice.getDevice();
            if (bondState == BluetoothDevice.BOND_NONE) {
                if (device.isBluetoothDock()) {
                    // After a dock is unpaired, we will forget the settings
                    LocalBluetoothPreferences
                            .removeDockAutoConnectSetting(mContext, device.getAddress());

                    // if the device is undocked, remove it from the list as well
                    if (!device.getAddress().equals(getDockedDeviceAddress(mContext))) {
                        //cachedDevice.setVisible(false);
                    }
                }
            }
        }

        // This can't be called from a broadcast receiver where the filter is set in the Manifest.
        private static String getDockedDeviceAddress(Context context) {
            // This works only because these broadcast intents are "sticky"
            Intent i = context.registerReceiver(null, new IntentFilter(Intent.ACTION_DOCK_EVENT));
            if (i != null) {
                int state = i.getIntExtra(Intent.EXTRA_DOCK_STATE, Intent.EXTRA_DOCK_STATE_UNDOCKED);
                if (state != Intent.EXTRA_DOCK_STATE_UNDOCKED) {
                    BluetoothDevice device = i.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    if (device != null) {
                        return device.getAddress();
                    }
                }
            }
            return null;
        }
    }
}
