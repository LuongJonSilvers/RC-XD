package com.example.rcxd.ui.screens

import android.os.Handler
import android.os.Looper
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.rcxd.bluetooth.ControllerServer.accelCrashDetected
import com.example.rcxd.bluetooth.ControllerServer.readLatitude
import com.example.rcxd.bluetooth.ControllerServer.readLongitude
import com.example.rcxd.bluetooth.ControllerServer.readNorthSouth
import com.example.rcxd.bluetooth.ControllerServer.readWestEast
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

class RemoteControllerViewModel : ViewModel() {
    private val uiState = MutableStateFlow(RemoteControllerUIState())
    val settingsUIStateStream: StateFlow<RemoteControllerUIState>
        get() = uiState

    init {
        viewModelScope.launch {
            val mainHandler = Handler(Looper.getMainLooper())

            mainHandler.post(object : Runnable {
                override fun run() {
                    accelCrashDetect()
                    readLatitude()
                    readLongitude()
                    readWestEast()
                    readNorthSouth()
                    mainHandler.postDelayed(this, 10000)
                }
            })
//            gpsUpdate()
//            accelCrashDetect()
            uiState.value = settingsUIStateStream.value.copy()
        }
    }

    fun onForward(magnitude: Byte, changeDirectionForward: Boolean) {
        if (changeDirectionForward) {
            uiState.update { it.copy(directionDriveTrain = Direction.FORWARD) }
        }
        uiState.update { it.copy(magnitudeDriveTrain = magnitude) }
    }

    fun onBackward(magnitude: Byte, changeDirectionBackward: Boolean) {
        if (changeDirectionBackward) {
            uiState.update { it.copy(directionDriveTrain = Direction.BACKWARD) }
        }
        uiState.update { it.copy(magnitudeDriveTrain = magnitude) }
    }

    fun onRight(magnitude: Byte, changeDirectionRight: Boolean) {
        if (changeDirectionRight) {
            uiState.update { it.copy(directionServo = Direction.R) }
        }
        uiState.update { it.copy(magnitudeServo = magnitude) }
    }

    fun onLeft(magnitude: Byte, changeDirectionLeft: Boolean) {
        if (changeDirectionLeft) {
            uiState.update { it.copy(directionServo = Direction.L) }
        }
        uiState.update { it.copy(magnitudeServo = magnitude) }
    }

    fun gpsUpdate() {
        viewModelScope.launch {
            if (readLatitude().isNotEmpty() && readLongitude().isNotEmpty() && readWestEast().isNotEmpty() && readNorthSouth().isNotEmpty()) {
                if(readLatitude()[0] == 0.toByte() && readLatitude()[1] == 0.toByte() && readLatitude()[2] == 0.toByte() && readLatitude()[3] == 0.toByte()) {
                    uiState.update { it.copy(gpsReady = false) }
                    uiState.update { it.copy(gpsLatitude = "1415 Engineering Drive, Madison, WI") }
                } else {
                    uiState.update { it.copy(gpsReady = true) }
                    uiState.update { it.copy(gpsLatitude = readLatitude()[0].toString() + readLatitude()[1].toString() +"."+readLatitude()[2].toString() + readLatitude()[3].toString()+"\u00B0") }
                    uiState.update { it.copy(gpsLongitude = readLongitude()[0].toString() + readLongitude()[1].toString() +"."+readLongitude()[2].toString() + readLongitude()[3].toString()+"\u00B0") }
                    if(readNorthSouth()[0] == 1.toByte()) {
                        uiState.update { it.copy(gpsLatitudeDirection = "N")}
                    } else {
                        uiState.update { it.copy(gpsLatitudeDirection = "S")}
                    }
                    if(readWestEast()[0] == 1.toByte()) {
                        uiState.update { it.copy(gpsLongitudeDirection = "E")}
                    } else {
                        uiState.update { it.copy(gpsLongitudeDirection = "W")}
                    }
                }
            } else {
                uiState.update { it.copy(gpsReady = false) }
                uiState.update { it.copy(gpsLatitude = "1415 Engineering Drive, Madison, WI") }
            }
        }
    }

    fun accelCrashDetect() {
        viewModelScope.launch {
            if (accelCrashDetected().isNotEmpty()) {
                if (accelCrashDetected()[0].toInt() != 0) {
                    uiState.update { it.copy(collisionDetected = accelCrashDetected()[0].toInt()) }
                } else {
                    uiState.update { it.copy(collisionDetected = 0) }
                }
            }
        }
    }

    fun dismiss() {
        uiState.update { it.copy(collisionDetected = 0) }
        uiState.value = settingsUIStateStream.value.copy()
    }
}