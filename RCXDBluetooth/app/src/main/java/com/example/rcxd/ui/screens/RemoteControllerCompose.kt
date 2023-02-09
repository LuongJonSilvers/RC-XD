package com.example.rcxd.ui.screens

import android.util.Log
import android.view.MotionEvent
import androidx.compose.foundation.Image
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.ExperimentalComposeUiApi
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.RectangleShape
import androidx.compose.ui.input.pointer.pointerInteropFilter
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.rcxd.bluetooth.ControllerServer
import com.example.rcxd.ui.theme.RepeatingButton
import com.example.rcxdbluetooth.R


private const val TAG = "RemoteControllerScreen"

const val MAX_VALUE = 0x64.toByte()
const val DRIVE_TRAIN_INCREMENT = 10.toByte()
const val SERVO_INCREMENT = 100.toByte()
const val SERVO_DECREMENT = 5.toByte()


@Composable
@Suppress("UnusedPrivateMember")
fun RemoteScreen(
    name: String,
    modifier: Modifier = Modifier,
    remoteControllerViewModel: RemoteControllerViewModel
) {
    val uiState by remoteControllerViewModel.settingsUIStateStream.collectAsState()
    RemoteControllerScreen(
        name = name,
        uiState = uiState,
        onForward = remoteControllerViewModel::onForward,
        onBackward = remoteControllerViewModel::onBackward,
        onRight = remoteControllerViewModel::onRight,
        onLeft = remoteControllerViewModel::onLeft,
        gpsUpdate = remoteControllerViewModel::gpsUpdate,
        accelCrashDetect = remoteControllerViewModel::accelCrashDetect
    )
}

@Composable
fun RemoteControllerScreen(
    name: String,
    modifier: Modifier = Modifier,
    uiState: RemoteControllerUIState,
    onForward: (Byte, Boolean) -> Unit,
    onBackward: (Byte, Boolean) -> Unit,
    onRight: (Byte, Boolean) -> Unit,
    onLeft: (Byte, Boolean) -> Unit,
    gpsUpdate: () -> Unit,
    accelCrashDetect: () -> Unit
) {

    Column(
        modifier = modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            modifier = modifier
                .fillMaxWidth()
                .border(
                    width = 1.dp,
                    color = MaterialTheme.colorScheme.outline,
                    shape = RectangleShape
                ),
            text = "Connected to: $name",
            fontSize = 30.sp
        )
        fireButton(uiState = uiState)
        Row(
            modifier = Modifier
                .fillMaxSize()
                .weight(1f),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column(
                horizontalAlignment = Alignment.Start,
                verticalArrangement = Arrangement.Center
            ) {
                forwardButton(
                    uiState = uiState,
                    onForward = onForward
                )
                backButton(
                    uiState = uiState,
                    onBack = onBackward
                )
            }
            leftButton(uiState, onLeft = onLeft)
            rightButton(uiState, onRight = onRight)
        }
        stopSign(uiState = uiState, onBack = onBackward, onForward = onForward)
        ShowMagnitudes(uiState = uiState)
        ShowGPSLocation(uiState = uiState, gpsUpdate = gpsUpdate)

        accelCrashDetect()
        if (uiState.collisionDetected != 0) {
            AlertDialog(
                onDismissRequest = {}, confirmButton = {},
                title = {
                    Box(
                        contentAlignment = Alignment.Center,
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Text(
                            modifier = modifier.align(Alignment.Center),
                            text = "Crash detected! Click below to close this popup"
                        )
                    }
                },
                dismissButton = {
                    Image(
                        painter = painterResource(R.drawable.red_x_1),
                        contentDescription = null,
                        modifier = modifier
                            .size(50.dp)
                            .clickable() {
                                while (!ControllerServer.accelAcknowledge()) {
                                }
                            },
                    )
                }
            )
        }
    }
}

@Composable
fun forwardButton(
    uiState: RemoteControllerUIState,
    modifier: Modifier = Modifier,
    onForward: (Byte, Boolean) -> Unit,
) {
    val magnitude = byteArrayOf(uiState.magnitudeDriveTrain)
    var changeDirectionForward = false

    RepeatingButton(
        modifier = modifier
            .size(125.dp)
            .padding(5.dp),
        shape = MaterialTheme.shapes.extraLarge,
        onClick = {
            if (uiState.magnitudeDriveTrain == 0x0.toByte()) {
                if (uiState.directionDriveTrain == Direction.BACKWARD) {
                    changeDirectionForward = true
                    onForward(magnitude[0], changeDirectionForward)
                } else if (uiState.directionDriveTrain == Direction.FORWARD) {
                    magnitude[0] = (magnitude[0] + DRIVE_TRAIN_INCREMENT).toByte()
                    onForward(magnitude[0], changeDirectionForward)
                    while (!ControllerServer.sendForwardControl(magnitude)) {
                    }
                }
            } else if (uiState.magnitudeDriveTrain > 0x0.toByte()) {
                if (uiState.directionDriveTrain == Direction.BACKWARD) {
                    magnitude[0] = (magnitude[0] - DRIVE_TRAIN_INCREMENT).toByte()
                    onForward(magnitude[0], changeDirectionForward)
                    while (!ControllerServer.sendBackwardControl(magnitude)) {
                    }
                } else if (uiState.magnitudeDriveTrain != MAX_VALUE && uiState.directionDriveTrain == Direction.FORWARD) {
                    magnitude[0] = (magnitude[0] + DRIVE_TRAIN_INCREMENT).toByte()
                    onForward(magnitude[0], changeDirectionForward)
                    while (!ControllerServer.sendForwardControl(magnitude)) {
                    }
                }
            }
            Log.i(TAG, "Forward Message sent, current mag: ${magnitude[0]}")
            Log.i(TAG, "Forward Message sent, current mag: ${uiState.directionDriveTrain}")
        },
    ) {
        Column(
            modifier = Modifier
                .padding(5.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            Image(imageVector = Icons.Default.KeyboardArrowUp, contentDescription = "Forward")
        }
    }
}

@Composable
fun backButton(
    uiState: RemoteControllerUIState,
    modifier: Modifier = Modifier,
    onBack: (Byte, Boolean) -> Unit,
) {
    val magnitude = byteArrayOf(uiState.magnitudeDriveTrain)
    var changeDirectionBack = false

    RepeatingButton(
        modifier = modifier
            .size(125.dp)
            .padding(5.dp),
        shape = MaterialTheme.shapes.extraLarge,
        onClick = {
            if (uiState.magnitudeDriveTrain == 0x0.toByte()) {
                if (uiState.directionDriveTrain == Direction.FORWARD) {
                    changeDirectionBack = true
                    onBack(magnitude[0], changeDirectionBack)
                } else if (uiState.directionDriveTrain == Direction.BACKWARD) {
                    magnitude[0] = (magnitude[0] + DRIVE_TRAIN_INCREMENT).toByte()
                    onBack(magnitude[0], changeDirectionBack)
                    while (!ControllerServer.sendBackwardControl(magnitude)) {
                    }
                }
            } else if (uiState.magnitudeDriveTrain > 0x0.toByte()) {
                if (uiState.directionDriveTrain == Direction.FORWARD) {
                    magnitude[0] = (magnitude[0] - DRIVE_TRAIN_INCREMENT).toByte()
                    onBack(magnitude[0], changeDirectionBack)
                    while (!ControllerServer.sendForwardControl(magnitude)) {
                    }
                } else if (uiState.magnitudeDriveTrain != MAX_VALUE && uiState.directionDriveTrain == Direction.BACKWARD) {
                    magnitude[0] = (magnitude[0] + DRIVE_TRAIN_INCREMENT).toByte()
                    onBack(magnitude[0], changeDirectionBack)
                    while (!ControllerServer.sendBackwardControl(magnitude)) {
                    }
                }
            }
            Log.i(TAG, "Back Message sent, current mag: ${magnitude[0]}")
            Log.i(TAG, "Back Message sent, current mag: ${uiState.directionDriveTrain}")
        },
    ) {
        Column(
            modifier = Modifier
                .padding(5.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            Image(imageVector = Icons.Default.KeyboardArrowDown, contentDescription = "Forward")
        }
    }
}

@OptIn(ExperimentalComposeUiApi::class)
@Composable
fun rightButton(
    uiState: RemoteControllerUIState,
    modifier: Modifier = Modifier,
    onRight: (Byte, Boolean) -> Unit,
) {
    val magnitude = byteArrayOf(uiState.magnitudeServo)
    var changeDirectionRight = false
    var enabled by remember { mutableStateOf(true) }
    var currentlyDec by remember { mutableStateOf(false) }

    Button(
        modifier = modifier
            .size(125.dp)
            .padding(5.dp)
            .pointerInteropFilter {
                when (it.action) {
                    MotionEvent.ACTION_DOWN -> {
                        if (!currentlyDec) {
                            enabled = false
                            if (uiState.magnitudeServo == 0x0.toByte()) {
                                if (uiState.directionServo == Direction.L) {
                                    changeDirectionRight = true
                                    magnitude[0] = (magnitude[0] + SERVO_INCREMENT).toByte()
                                    onRight(magnitude[0], changeDirectionRight)
                                    ControllerServer.sendRightControl(magnitude)
                                } else if (uiState.directionServo == Direction.R) {
                                    magnitude[0] = (magnitude[0] + SERVO_INCREMENT).toByte()
                                    onRight(magnitude[0], changeDirectionRight)
                                    ControllerServer.sendRightControl(magnitude)
                                }
                            }
                            Log.i(TAG, "Right Message sent, current mag: ${magnitude[0]}")
                            Log.i(TAG, "Right Message sent, current mag: ${uiState.directionServo}")
                        }
                    }
                    MotionEvent.ACTION_UP -> {
                        if (!currentlyDec) {
                            currentlyDec = true
                            for (i in 1..18) {
                                if (uiState.magnitudeServo > 0x0.toByte()) {
                                    magnitude[0] = (magnitude[0] - SERVO_DECREMENT).toByte()
                                    onRight(magnitude[0], changeDirectionRight)
                                    while (!ControllerServer.sendRightControl(magnitude)) {
                                    }
                                }
                                if (magnitude[0] == 10.toByte()) {
                                    magnitude[0] = (magnitude[0] - 10).toByte()
                                    onRight(magnitude[0], changeDirectionRight)
                                    ControllerServer.sendRightControl(magnitude)
                                    while (!ControllerServer.sendRightControl(magnitude)) {
                                    }
                                }
                                Log.i(TAG, "Right Message sent, current mag: ${magnitude[0]}")
                                Log.i(
                                    TAG,
                                    "Right Message sent, current mag: ${uiState.directionServo}"
                                )
                            }
                            while (!ControllerServer.sendServoZeroSignal()) {
                            }
                            enabled = true
                            currentlyDec = false
                        }
                    }
                }
                true
            },
        shape = MaterialTheme.shapes.extraLarge,
        onClick = {},
        enabled = enabled
    ) {
        Column(
            modifier = Modifier
                .padding(5.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            Image(imageVector = Icons.Default.KeyboardArrowRight, contentDescription = "Forward")
        }
    }
}


@OptIn(ExperimentalComposeUiApi::class)
@Composable
fun leftButton(
    uiState: RemoteControllerUIState,
    modifier: Modifier = Modifier,
    onLeft: (Byte, Boolean) -> Unit,
) {
    val magnitude = byteArrayOf(uiState.magnitudeServo)
    var changeDirectionLeft = false
    var enabled by remember { mutableStateOf(true) }
    var currentlyDec by remember { mutableStateOf(false) }

    Button(
        modifier = modifier
            .size(125.dp)
            .padding(5.dp)
            .pointerInteropFilter {
                when (it.action) {
                    MotionEvent.ACTION_DOWN -> {
                        if (!currentlyDec) {
                            enabled = false
                            if (uiState.magnitudeServo == 0x0.toByte()) {
                                if (uiState.directionServo == Direction.R) {
                                    changeDirectionLeft = true
                                    magnitude[0] = (magnitude[0] + SERVO_INCREMENT).toByte()
                                    onLeft(magnitude[0], changeDirectionLeft)
                                    ControllerServer.sendLeftControl(magnitude)
                                } else if (uiState.directionServo == Direction.L) {
                                    magnitude[0] = (magnitude[0] + SERVO_INCREMENT).toByte()
                                    onLeft(magnitude[0], changeDirectionLeft)
                                    ControllerServer.sendLeftControl(magnitude)
                                }
                            }
                            Log.i(TAG, "Left Message sent, current mag: ${magnitude[0]}")
                            Log.i(TAG, "Left Message sent, current mag: ${uiState.directionServo}")
                        }
                    }
                    MotionEvent.ACTION_UP -> {
                        if (!currentlyDec) {
                            currentlyDec = true
                            var successfullySent = false
                            for (i in 1..18) {
                                Log.i(TAG, "ready to turn: $enabled")
                                if (uiState.magnitudeServo > 0x0.toByte()) {
                                    magnitude[0] = (magnitude[0] - SERVO_DECREMENT).toByte()
                                    onLeft(magnitude[0], changeDirectionLeft)
                                    while (!ControllerServer.sendLeftControl(magnitude)) {
                                    }
                                }

                                if (magnitude[0] == 10.toByte()) {
                                    magnitude[0] = (magnitude[0] - 10).toByte()
                                    onLeft(magnitude[0], changeDirectionLeft)
                                    while (!ControllerServer.sendLeftControl(magnitude)) {
                                        successfullySent = true
                                    }
                                }
                                Log.i(TAG, "Left Message sent, current mag: ${magnitude[0]}")
                                Log.i(
                                    TAG,
                                    "Left Message sent, current mag: ${uiState.directionServo}"
                                )
                            }
                            if (successfullySent) {
                                Log.i(TAG, "Zero signal sent")
                                while (!ControllerServer.sendServoZeroSignal()) {
                                }
                            }
                            enabled = true
                            currentlyDec = false
                        }
                    }
                }
                true
            },
        shape = MaterialTheme.shapes.extraLarge,
        onClick = {},
        enabled = enabled
    ) {
        Column(
            modifier = Modifier
                .padding(5.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
            Image(imageVector = Icons.Default.KeyboardArrowLeft, contentDescription = "Forward")
        }
    }
}

@Composable
fun stopSign(
    uiState: RemoteControllerUIState,
    modifier: Modifier = Modifier,
    onBack: (Byte, Boolean) -> Unit,
    onForward: (Byte, Boolean) -> Unit,
) {
    val magnitude = byteArrayOf(uiState.magnitudeDriveTrain)
    val direction = uiState.directionDriveTrain
    Row(
        modifier = Modifier
            .fillMaxWidth(),
        horizontalArrangement = Arrangement.End,
        verticalAlignment = Alignment.Top
    ) {
        Image(
            modifier = modifier
                .size(100.dp)
                .padding(8.dp)
                .clickable() {
                    while (magnitude[0] != 0x0.toByte()) {
                        if (uiState.directionDriveTrain == Direction.BACKWARD) {
                            magnitude[0] = (magnitude[0] - DRIVE_TRAIN_INCREMENT).toByte()
                            onForward(magnitude[0], false)
                            while (!ControllerServer.sendBackwardControl(magnitude)) {
                            }
                        } else if (uiState.directionDriveTrain == Direction.FORWARD) {
                            magnitude[0] = (magnitude[0] - DRIVE_TRAIN_INCREMENT).toByte()
                            onForward(magnitude[0], false)
                            while (!ControllerServer.sendForwardControl(magnitude)) {
                            }
                        }
                        Log.i(TAG, "Stop sign pressed, current mag: ${magnitude[0]}")
                        Log.i(TAG, "Stop sign pressed, current mag: $direction")
                    }
                    onForward(magnitude[0], true)
                },
            contentDescription = null,
            painter = painterResource(R.drawable.stop_sign),
        )
    }
}

@OptIn(ExperimentalComposeUiApi::class)
@Composable
fun fireButton(
    uiState: RemoteControllerUIState,
    modifier: Modifier = Modifier
) {
    val magnitude = byteArrayOf(uiState.magnitudeDriveTrain)
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(4.dp),
        horizontalArrangement = Arrangement.End,
        verticalAlignment = Alignment.Top
    ) {
        Button(
            modifier = modifier.pointerInteropFilter {
                when (it.action) {
                    MotionEvent.ACTION_DOWN -> {
                        magnitude[0] = 1.toByte()
                        while (!ControllerServer.turretFire(magnitude)) {
                        }

                    }
                    MotionEvent.ACTION_UP -> {
                        magnitude[0] = 0.toByte()
                        while (!ControllerServer.turretFire(magnitude)) {
                        }
                    }
                }
                true
            },
            onClick = {},
            colors = ButtonDefaults.buttonColors(containerColor = Color.Transparent)
        ) {
            Image(
                painter = painterResource(R.drawable.fire),
                modifier = modifier
                    .size(100.dp),
                contentDescription = null,
            )
        }
    }
}


@Composable
fun ShowMagnitudes(modifier: Modifier = Modifier, uiState: RemoteControllerUIState) {
    Column(
        horizontalAlignment = Alignment.Start,
        verticalArrangement = Arrangement.Top
    ) {
        Row {
            Box(
                modifier = Modifier
                    .border(
                        width = 1.dp, color = MaterialTheme.colorScheme.outline,
                        shape = RectangleShape
                    )
                    .padding(4.dp)
                    .size(width = 200.dp, height = 40.dp),
                contentAlignment = Alignment.Center
            ) {
                Column {
                    Text(
                        text = "Drive train magnitude: "
                    )
                    Text(
                        text = "${uiState.magnitudeDriveTrain}" + ", " + "${uiState.directionDriveTrain}"
                    )
                }
            }
            Box(
                modifier = Modifier
                    .border(
                        width = 1.dp, color = MaterialTheme.colorScheme.outline,
                        shape = RectangleShape
                    )
                    .padding(4.dp)
                    .size(width = 180.dp, height = 40.dp),
                contentAlignment = Alignment.Center
            ) {
                Column {
                    Text(
                        text = "Servo magnitude: "
                    )
                    Text(
                        text = "${uiState.magnitudeServo}"
                    )
                }

            }
        }
    }
}

@Composable
fun ShowGPSLocation(
    modifier: Modifier = Modifier,
    uiState: RemoteControllerUIState,
    gpsUpdate: () -> Unit
) {
    gpsUpdate()
    Column(
        horizontalAlignment = Alignment.Start,
        verticalArrangement = Arrangement.Top
    ) {
        Row(
            modifier
                .fillMaxWidth()
                .border(
                    width = 1.dp, color = MaterialTheme.colorScheme.outline,
                    shape = RectangleShape
                )
        ) {
            Text(
                modifier = modifier
                    .padding(4.dp),
                text = "GPS Location: ",
            )
            val color: Color = if (uiState.gpsReady) {
                Color.Unspecified
            } else {
                Color.Red
            }

            val text: String = if (uiState.gpsReady) {
                uiState.gpsLatitude + " " + uiState.gpsLatitudeDirection + ", " + uiState.gpsLongitude + " " + uiState.gpsLongitudeDirection
            } else {
                uiState.gpsLatitude
            }

            Text(
                modifier = modifier
                    .padding(4.dp),
                text = text,
                color = color
            )
        }
    }
}


@Preview(
    showBackground = true
)
@Composable
fun ButtonPreviews() {
    RemoteControllerScreen(
        name = "RCXD",
        uiState = RemoteControllerUIState(),
        onRight = { _, _ -> },
        onLeft = { _, _ -> },
        onForward = { _, _ -> },
        onBackward = { _, _ -> },
        gpsUpdate = {},
        accelCrashDetect = {}
    )
}
