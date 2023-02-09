package com.example.rcxd.ui.screens

data class RemoteControllerUIState(
    val directionDriveTrain: Direction = Direction.FORWARD,
    val directionServo: Direction = Direction.R,
    val magnitudeDriveTrain: Byte = 0x0.toByte(),
    val magnitudeServo: Byte = 0x0.toByte(),
    val gpsLatitude: String = "",
    val gpsLatitudeDirection: String = "",
    val gpsLongitude: String = "",
    val gpsLongitudeDirection: String = "",
    val gpsReady: Boolean = false,
    val readyToTurn: Boolean = true,
    val collisionDetected: Int = 0
)


enum class Direction {
    FORWARD, BACKWARD, R, L
}
