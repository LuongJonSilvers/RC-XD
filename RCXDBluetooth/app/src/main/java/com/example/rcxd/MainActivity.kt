package com.example.rcxd

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.content.Intent
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.annotation.RequiresApi
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.rcxd.bluetooth.ControllerServer
import com.example.rcxd.bluetooth.DeviceScanViewModel
import com.example.rcxd.states.DeviceConnectionState
import com.example.rcxd.ui.screens.DeviceScanCompose
import com.example.rcxd.ui.screens.RemoteControllerViewModel
import com.example.rcxd.ui.screens.RemoteScreen
import com.example.rcxd.ui.theme.RCXDBluetoothTheme
import com.karumi.dexter.Dexter
import com.karumi.dexter.MultiplePermissionsReport
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.multi.MultiplePermissionsListener


class MainActivity : ComponentActivity() {

    private val viewModel: DeviceScanViewModel by viewModels()

    override fun onStop() {
        super.onStop()
        ControllerServer.stopServer()
    }

    @OptIn(ExperimentalMaterial3Api::class)
    @RequiresApi(Build.VERSION_CODES.S)
    @SuppressLint("MissingPermission")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            RCXDBluetoothTheme {
                val result = remember { mutableStateOf<Int?>(100) }
                val launcher =
                    rememberLauncherForActivityResult(ActivityResultContracts.StartActivityForResult()) {
                        result.value = it.resultCode
                    }

                LaunchedEffect(key1 = true) {

                    Dexter.withContext(this@MainActivity)
                        .withPermissions(
                            Manifest.permission.ACCESS_COARSE_LOCATION,
                            Manifest.permission.ACCESS_FINE_LOCATION,
                            Manifest.permission.BLUETOOTH_ADVERTISE,
                            Manifest.permission.BLUETOOTH_CONNECT,
                            Manifest.permission.BLUETOOTH_SCAN,
                            Manifest.permission.BLUETOOTH,
                            Manifest.permission.BLUETOOTH_ADMIN,
                            Manifest.permission.BLUETOOTH_PRIVILEGED,
                            )
                        .withListener(object : MultiplePermissionsListener {
                            override fun onPermissionsChecked(report: MultiplePermissionsReport) {
                                val intent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                                launcher.launch(intent)
                            }

                            override fun onPermissionRationaleShouldBeShown(
                                permissions: List<PermissionRequest?>?,
                                token: PermissionToken?
                            ) {
                            }
                        }).check()
                }

                LaunchedEffect(key1 = result.value) {
                    if (result.value == RESULT_OK) {
                        ControllerServer.startServer(application)
                        viewModel.startScan()
                    }
                }

                Scaffold(
                    topBar = {
                        TopAppBar(
                            title = {
                                Text(text = "RCXD App")
                            },
                            colors = TopAppBarDefaults.centerAlignedTopAppBarColors(
                                containerColor = MaterialTheme.colorScheme.primaryContainer
                            )
                        )
                    },
                    content = { padding ->
                        Surface(
                            modifier = Modifier
                                .fillMaxSize()
                                .padding(padding),
                            color = MaterialTheme.colorScheme.background
                        ) {
                            val deviceScanningState by viewModel.viewState.observeAsState()

                            val deviceConnectionState by ControllerServer.deviceConnection.observeAsState()

                            var isChatOpen by remember {
                                mutableStateOf(false)
                            }

                            Box(
                                contentAlignment = Alignment.TopCenter,
                                modifier = Modifier.fillMaxSize()
                            ) {
                                if (deviceScanningState != null && !isChatOpen
                                    || deviceConnectionState == DeviceConnectionState.Disconnected
                                ) {
                                    Column {
                                        Text(
                                            text = "Choose a device to chat with:",
                                            fontSize = 20.sp,
                                            fontWeight = FontWeight.Bold
                                        )
                                        Spacer(modifier = Modifier.height(10.dp))
                                        DeviceScanCompose.DeviceScan(deviceScanViewState = deviceScanningState!!) {
                                            isChatOpen = true
                                        }
                                    }
                                } else if (deviceScanningState != null && deviceConnectionState is DeviceConnectionState.Connected) {
                                    RemoteScreen(
                                        name = (deviceConnectionState as DeviceConnectionState.Connected).device.name?: "unknown",
                                        remoteControllerViewModel = RemoteControllerViewModel()
                                    )
                                } else {
                                    Text(text = "No devices")
                                }
                            }
                        }
                    }
                )
            }
        }
    }
}
