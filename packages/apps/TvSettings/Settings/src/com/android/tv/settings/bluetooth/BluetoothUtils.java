/**
 *
 */
package com.android.tv.settings.bluetooth;

import android.app.AlertDialog;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.Toast;

import com.android.tv.settings.R;
import com.android.tv.settings.bluetooth.DockService.DockBluetoothCallback;
//import com.android.tv.settings.search.Index;
//import com.android.tv.settings.search.SearchIndexableRaw;
import com.android.settingslib.bluetooth.LocalBluetoothManager;
import com.android.settingslib.bluetooth.LocalBluetoothManager.BluetoothManagerCallback;
import com.android.settingslib.bluetooth.Utils.ErrorListener;

import android.os.UserHandle;
import android.os.UserManager;
import android.content.pm.UserInfo;

/**
 * @author GaoFei
 */
public final class BluetoothUtils {
    static final boolean V = com.android.settingslib.bluetooth.Utils.V; // verbose logging
    static final boolean D = com.android.settingslib.bluetooth.Utils.D;  // regular logging

    private BluetoothUtils() {
    }

    public static int getConnectionStateSummary(int connectionState) {
        switch (connectionState) {
            case BluetoothProfile.STATE_CONNECTED:
                return R.string.bluetooth_connected;
            case BluetoothProfile.STATE_CONNECTING:
                return R.string.bluetooth_connecting;
            case BluetoothProfile.STATE_DISCONNECTED:
                return R.string.bluetooth_disconnected;
            case BluetoothProfile.STATE_DISCONNECTING:
                return R.string.bluetooth_disconnecting;
            default:
                return 0;
        }
    }

    // Create (or recycle existing) and show disconnect dialog.
    static AlertDialog showDisconnectDialog(Context context,
                                            AlertDialog dialog,
                                            DialogInterface.OnClickListener disconnectListener,
                                            CharSequence title, CharSequence message) {
        if (dialog == null) {
            dialog = new AlertDialog.Builder(context)
                    .setPositiveButton(android.R.string.ok, disconnectListener)
                    .setNegativeButton(android.R.string.cancel, null)
                    .create();
        } else {
            if (dialog.isShowing()) {
                dialog.dismiss();
            }
            // use disconnectListener for the correct profile(s)
            CharSequence okText = context.getText(android.R.string.ok);
            dialog.setButton(DialogInterface.BUTTON_POSITIVE,
                    okText, disconnectListener);
        }
        dialog.setTitle(title);
        dialog.setMessage(message);
        dialog.show();
        return dialog;
    }

    // TODO: wire this up to show connection errors...
    static void showConnectingError(Context context, String name) {
        // if (!mIsConnectingErrorPossible) {
        //     return;
        // }
        // mIsConnectingErrorPossible = false;

        showError(context, name, R.string.bluetooth_connecting_error_message);
    }

    static void showError(Context context, String name, int messageResId) {
        String message = context.getString(messageResId, name);
        LocalBluetoothManager manager = getLocalBtManager(context);
        Context activity = manager.getForegroundActivity();
        if (manager.isForegroundActivity()) {
            new AlertDialog.Builder(activity)
                    .setTitle(R.string.bluetooth_error_title)
                    .setMessage(message)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
        } else {
            Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Update the search Index for a specific class name and resources.
     */
    public static void updateSearchIndex(Context context, String className, String title,
                                         String screenTitle, int iconResId, boolean enabled) {
        //SearchIndexableRaw data = new SearchIndexableRaw(context);
        //data.className = className;
        //data.title = title;
        //data.screenTitle = screenTitle;
        //data.iconResId = iconResId;
        //data.enabled = enabled;

        //Index.getInstance(context).updateFromSearchIndexableData(data);
    }

    public static LocalBluetoothManager getLocalBtManager(Context context) {
        return LocalBluetoothManager.getInstance(context, mOnInitCallback);
    }

    private static final ErrorListener mErrorListener = new ErrorListener() {
        @Override
        public void onShowError(Context context, String name, int messageResId) {
            showError(context, name, messageResId);
        }
    };

    private static final BluetoothManagerCallback mOnInitCallback = new BluetoothManagerCallback() {
        @Override
        public void onBluetoothManagerInitialized(Context appContext,
                                                  LocalBluetoothManager bluetoothManager) {
            bluetoothManager.getEventManager().registerCallback(
                    new DockBluetoothCallback(appContext));
            com.android.settingslib.bluetooth.Utils.setErrorListener(mErrorListener);
        }
    };

    public static boolean isManagedProfile(UserManager userManager) {
        return isManagedProfile(userManager, UserHandle.myUserId());
    }

    public static boolean isManagedProfile(UserManager userManager, int userId) {
        if (userManager == null) {
            throw new IllegalArgumentException("userManager must not be null");
        }
        UserInfo userInfo = userManager.getUserInfo(userId);
        return (userInfo != null) ? userInfo.isManagedProfile() : false;
    }
}
