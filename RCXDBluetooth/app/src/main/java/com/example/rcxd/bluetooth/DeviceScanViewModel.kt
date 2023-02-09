package com.example.rcxd.bluetooth

import android.annotation.SuppressLint
import android.app.Application
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.le.*
import android.os.Handler
import android.util.Log
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.example.rcxd.states.DeviceScanViewState


private const val TAG = "DeviceScanViewModel"

private const val SCAN_PERIOD = 20000L

class DeviceScanViewModel(app: Application) : AndroidViewModel(app) {

    private val _viewState = MutableLiveData<DeviceScanViewState>()
    val viewState = _viewState as LiveData<DeviceScanViewState>

    private val scanResults = mutableMapOf<String, BluetoothDevice>()

    private val adapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

    private var scanner: BluetoothLeScanner? = null

    private var scanCallback: DeviceScanCallback? = null
    private lateinit var scanFilters: List<ScanFilter>
    private lateinit var scanSettings: ScanSettings

    override fun onCleared() {
        super.onCleared()
        stopScanning()
    }

    @SuppressLint("MissingPermission")
    fun startScan() {
        scanFilters = buildScanFilters()
        scanSettings = buildScanSettings()
        if (!adapter.isMultipleAdvertisementSupported) {
            _viewState.value = DeviceScanViewState.AdvertisementNotSupported
            return
        }

        if (scanCallback == null) {
            scanner = adapter.bluetoothLeScanner
            _viewState.value = DeviceScanViewState.ActiveScan
            Handler().postDelayed({ stopScanning() }, SCAN_PERIOD)

            scanCallback = DeviceScanCallback()
            scanner?.startScan(scanFilters, scanSettings, scanCallback)
        }
    }

    @SuppressLint("MissingPermission")
    fun stopScanning() {
        scanner?.stopScan(scanCallback)
        scanCallback = null
        _viewState.value = DeviceScanViewState.ScanResults(scanResults)
    }

    private fun buildScanFilters(): List<ScanFilter> {
        val builder = ScanFilter.Builder()
//        builder.setDeviceAddress("00:A0:50:C2:E4:28") // old psoc
        builder.setDeviceAddress("00:A0:50:54:D6:8E") // new psoc
        val filter = builder.build()
        return listOf(filter)
    }


    private fun buildScanSettings(): ScanSettings {
        return ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()
    }


    private inner class DeviceScanCallback : ScanCallback() {
        override fun onBatchScanResults(results: List<ScanResult>) {
            super.onBatchScanResults(results)
            for (item in results) {
                item.device?.let { device ->
                    scanResults[device.address] = device
                    if (item.device.address.equals("00:A0:50:C2:E4:28")) {
                        Log.i(TAG, "PSOC Found in batch results")
                        _viewState.value = DeviceScanViewState.ScanResults(scanResults)
                        stopScanning()
                    }
                }
            }
            Log.i(TAG, scanResults.toString())
            _viewState.value = DeviceScanViewState.ScanResults(scanResults)
        }

        override fun onScanResult(
            callbackType: Int,
            result: ScanResult
        ) {
            super.onScanResult(callbackType, result)
            result.device?.let { device ->
                scanResults[device.address] = device
                if (result.device.address.equals("00:A0:50:C2:E4:28")) {
                    Log.i(TAG, "PSOC Found in scan result")
                    _viewState.value = DeviceScanViewState.ScanResults(scanResults)
                    stopScanning()
                }
            }
            if (result.device.address.equals("00:A0:50:C2:E4:28")) {
                Log.i(TAG, "PSOC Found after scan result")
            }
            Log.i(TAG, scanResults.toString())
            _viewState.value = DeviceScanViewState.ScanResults(scanResults)
        }

        override fun onScanFailed(errorCode: Int) {
            super.onScanFailed(errorCode)
            val errorMessage = "Scan failed with error: $errorCode"
            _viewState.value = DeviceScanViewState.Error(errorMessage)
        }
    }
}