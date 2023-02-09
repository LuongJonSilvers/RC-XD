package com.example.rcxd.bluetooth

import android.annotation.SuppressLint
import android.app.Application
import android.bluetooth.*
import android.bluetooth.le.AdvertiseCallback
import android.bluetooth.le.AdvertiseData
import android.bluetooth.le.AdvertiseSettings
import android.bluetooth.le.BluetoothLeAdvertiser
import android.content.Context
import android.os.ParcelUuid
import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.example.rcxd.*
import com.example.rcxd.states.DeviceConnectionState
import com.example.rcxd.ui.screens.RemoteControllerViewModel
import java.util.*

private const val TAG = "ControllerServerTag"

object ControllerServer {

    private var app: Application? = null
    private lateinit var bluetoothManager: BluetoothManager

    private var adapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

    private var advertiser: BluetoothLeAdvertiser? = null
    private var advertiseCallback: AdvertiseCallback? = null
    private var advertiseSettings: AdvertiseSettings = buildAdvertiseSettings()
    private var advertiseData: AdvertiseData = buildAdvertiseData()

    private var gattServer: BluetoothGattServer? = null
    private var gattServerCallback: BluetoothGattServerCallback? = null

    private var gattClient: BluetoothGatt? = null
    private var gattClientCallback: BluetoothGattCallback? = null

    private var currentDevice: BluetoothDevice? = null

    private val _deviceConnection = MutableLiveData<DeviceConnectionState>()
    val deviceConnection = _deviceConnection as LiveData<DeviceConnectionState>

    private var gattForward: BluetoothGatt? = null
    private var gattBackward: BluetoothGatt? = null
    private var gattRight: BluetoothGatt? = null
    private var gattLeft: BluetoothGatt? = null
    private var gattServoZero: BluetoothGatt? = null
    private var gattLatitude: BluetoothGatt? = null
    private var gattLongitude: BluetoothGatt? = null
    private var gattNorthSouth: BluetoothGatt? = null
    private var gattWestEast: BluetoothGatt? = null
    private var gattAccelCrash: BluetoothGatt? = null
    private var gattAccelAcknowledge: BluetoothGatt? = null
    private var gattTurret: BluetoothGatt? = null

    private var forwardCharacteristic: BluetoothGattCharacteristic? = null
    private var backwardCharacteristic: BluetoothGattCharacteristic? = null
    private var rightCharacteristic: BluetoothGattCharacteristic? = null
    private var leftCharacteristic: BluetoothGattCharacteristic? = null
    private var servoZeroCharacteristic: BluetoothGattCharacteristic? = null
    private var latitudeCharacteristic: BluetoothGattCharacteristic? = null
    private var longitudeCharacteristic: BluetoothGattCharacteristic? = null
    private var northSouthCharacteristic: BluetoothGattCharacteristic? = null
    private var westEastCharacteristic: BluetoothGattCharacteristic? = null
    private var accelCrashCharacteristic: BluetoothGattCharacteristic? = null
    private var accelAcknowledgeCharacteristic: BluetoothGattCharacteristic? = null
    private var turretCharacteristic: BluetoothGattCharacteristic? = null

    fun startServer(app: Application) {
        bluetoothManager = app.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        adapter = bluetoothManager.adapter
        setupGattServer(app)
        startAdvertisement()
        RemoteControllerViewModel().gpsUpdate()
        RemoteControllerViewModel().accelCrashDetect()
    }

    fun stopServer() {
        stopAdvertising()
    }

    @SuppressLint("MissingPermission")
    fun setupGattServer(app: Application) {

        gattServerCallback = GattServerCallback()

        gattServer = bluetoothManager.openGattServer(
            app,
            gattServerCallback
        ).apply {
            addService(setupWriteGattService(FORWARD_SERVICE_UUID, FORWARD_CHARACTERISTIC_UUID))
            addService(setupWriteGattService(BACKWARD_SERVICE_UUID, BACKWARD_CHARACTERISTIC_UUID))
            addService(setupWriteGattService(RIGHT_SERVICE_UUID, RIGHT_CHARACTERISTIC_UUID))
            addService(setupWriteGattService(LEFT_SERVICE_UUID, LEFT_CHARACTERISTIC_UUID))
            addService(setupWriteGattService(SERVO_ZERO_SERVICE_UUID, SERVO_ZERO_CHARACTERISTIC_UUID))
            addService(setupReadGattService(LATITUDE_SERVICE_UUID, LATITUDE_CHARACTERISTIC_UUID))
            addService(setupReadGattService(LONGITUDE_SERVICE_UUID, LONGITUDE_CHARACTERISTIC_UUID))
            addService(setupReadGattService(NORTHSOUTH_SERVICE_UUID, NORTHSOUTH_CHARACTERISTIC_UUID))
            addService(setupReadGattService(WESTEAST_SERVICE_UUID, WESTEAST_CHARACTERISTIC_UUID))
            addService(setupReadGattService(ACCEL_CRASH_SERVICE_UUID, ACCEL_CRASH_CHARACTERISTIC_UUID))
            addService(setupWriteGattService(ACCEL_ACKNOWLEDGE_SERVICE_UUID, ACCEL_ACKNOWLEDGE_CHARACTERISTIC_UUID))
            addService(setupWriteGattService(TURRET_SERVICE_UUID, TURRET_CHARACTERISTIC_UUID))
        }
    }

    private class GattServerCallback : BluetoothGattServerCallback() {
        override fun onConnectionStateChange(device: BluetoothDevice, status: Int, newState: Int) {
            super.onConnectionStateChange(device, status, newState)
            val isSuccess = status == BluetoothGatt.GATT_SUCCESS
            val isConnected = newState == BluetoothProfile.STATE_CONNECTED
            if (isSuccess && isConnected) {
                setCurrentConnection(device)
            } else {
                Log.i(TAG, "deviceConnection: $deviceConnection")
                _deviceConnection.postValue(DeviceConnectionState.Disconnected)
            }
        }

        @SuppressLint("MissingPermission")
        override fun onCharacteristicWriteRequest(
            device: BluetoothDevice,
            requestId: Int,
            characteristic: BluetoothGattCharacteristic,
            preparedWrite: Boolean,
            responseNeeded: Boolean,
            offset: Int,
            value: ByteArray?
        ) {
            super.onCharacteristicWriteRequest(
                device,
                requestId,
                characteristic,
                preparedWrite,
                responseNeeded,
                offset,
                value
            )
            Log.i(TAG, "Gatt server write req: $characteristic")
            if (characteristic.uuid == LATITUDE_CHARACTERISTIC_UUID) {
                gattServer?.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, null)
                value?.let {
                    Log.i(TAG, "Gatt server value: ${it.size}")
                }
            }
        }

        @SuppressLint("MissingPermission")
        override fun onCharacteristicReadRequest(
            device: BluetoothDevice?,
            requestId: Int,
            offset: Int,
            characteristic: BluetoothGattCharacteristic?
        ) {
            super.onCharacteristicReadRequest(device, requestId, offset, characteristic)
            Log.i(TAG, "Gatt server read req: $characteristic")
            if (characteristic?.uuid == LATITUDE_CHARACTERISTIC_UUID) {
                Log.i(TAG, "Gatt server read:")
                gattServer?.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, null)
            }
        }
    }

    fun setCurrentConnection(device: BluetoothDevice) {
        currentDevice = device
        _deviceConnection.postValue(DeviceConnectionState.Connected(device))
        connectToDevice(device)
    }

    @SuppressLint("MissingPermission")
    private fun connectToDevice(device: BluetoothDevice) {
        gattClientCallback = GattClientCallback()
        gattClient = device.connectGatt(app, false, gattClientCallback)
    }

    private fun setupWriteGattService(
        SERVICE_UUID: UUID,
        CHARACTERISTIC_UUID: UUID
    ): BluetoothGattService {
        val service = BluetoothGattService(SERVICE_UUID, BluetoothGattService.SERVICE_TYPE_PRIMARY)
        val messageCharacteristic = BluetoothGattCharacteristic(
            CHARACTERISTIC_UUID,
            BluetoothGattCharacteristic.PROPERTY_WRITE,
            BluetoothGattCharacteristic.PERMISSION_WRITE
        )
        service.addCharacteristic(messageCharacteristic)

        return service
    }

    private fun setupReadGattService(
        SERVICE_UUID: UUID,
        CHARACTERISTIC_UUID: UUID
    ): BluetoothGattService {
        val service = BluetoothGattService(SERVICE_UUID, BluetoothGattService.SERVICE_TYPE_PRIMARY)
        val messageCharacteristic = BluetoothGattCharacteristic(
            CHARACTERISTIC_UUID,
            BluetoothGattCharacteristic.PROPERTY_READ,
            BluetoothGattCharacteristic.PERMISSION_READ
        )
        service.addCharacteristic(messageCharacteristic)
        return service
    }

    @SuppressLint("MissingPermission")
    private class GattClientCallback : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            super.onConnectionStateChange(gatt, status, newState)
            val isSuccess = status == BluetoothGatt.GATT_SUCCESS
            val isConnected = newState == BluetoothProfile.STATE_CONNECTED

            if (isSuccess && isConnected) {
                gatt.discoverServices()
            }
        }

        override fun onServicesDiscovered(discoveredGatt: BluetoothGatt, status: Int) {
            super.onServicesDiscovered(discoveredGatt, status)
            if (status == BluetoothGatt.GATT_SUCCESS) {
                gattForward = discoveredGatt
                val forwardService = discoveredGatt.getService(FORWARD_SERVICE_UUID)
                if (forwardService != null) {
                    forwardCharacteristic = forwardService.getCharacteristic(
                        FORWARD_CHARACTERISTIC_UUID
                    )
                    if (forwardCharacteristic == null) {
                        Log.i(TAG, "forwardCharacteristic null")
                    } else {
                        Log.i(TAG, "forwardCharacteristic setup")
                    }
                }

                gattBackward = discoveredGatt
                val backwardService = discoveredGatt.getService(BACKWARD_SERVICE_UUID)
                if (backwardService != null) {
                    backwardCharacteristic =
                        backwardService.getCharacteristic(BACKWARD_CHARACTERISTIC_UUID)
                    if (forwardCharacteristic == null) {
                        Log.i(TAG, "backwardCharacteristic null")
                    } else {
                        Log.i(TAG, "backwardCharacteristic setup")
                    }
                }

                gattRight = discoveredGatt
                val rightService = discoveredGatt.getService(RIGHT_SERVICE_UUID)
                if (rightService != null) {
                    rightCharacteristic =
                        rightService.getCharacteristic(RIGHT_CHARACTERISTIC_UUID)
                    if (forwardCharacteristic == null) {
                        Log.i(TAG, "rightCharacteristic null")
                    } else {
                        Log.i(TAG, "rightCharacteristic setup")
                    }
                }

                gattLeft = discoveredGatt
                val leftService = discoveredGatt.getService(LEFT_SERVICE_UUID)
                if (leftService != null) {
                    leftCharacteristic =
                        leftService.getCharacteristic(LEFT_CHARACTERISTIC_UUID)
                    if (forwardCharacteristic == null) {
                        Log.i(TAG, "leftCharacteristic null")
                    } else {
                        Log.i(TAG, "leftCharacteristic setup")
                        Log.i(TAG, "$leftCharacteristic!!.properties")
                    }
                }

                gattServoZero = discoveredGatt
                val servoZeroService = discoveredGatt.getService(SERVO_ZERO_SERVICE_UUID)
                if (servoZeroService != null) {
                    servoZeroCharacteristic =
                        servoZeroService.getCharacteristic(SERVO_ZERO_CHARACTERISTIC_UUID)
                    if (servoZeroCharacteristic == null) {
                        Log.i(TAG, "servo zero null")
                    } else {
                        Log.i(TAG, "servo zero setup")
                        val gpsChar = servoZeroCharacteristic!!.properties
                        Log.i(TAG, "$gpsChar")
                        val gpsCharPerm = servoZeroCharacteristic!!.permissions
                        Log.i(TAG, "$gpsCharPerm")
                    }
                }

                gattLatitude = discoveredGatt
                val gpsService = discoveredGatt.getService(LATITUDE_SERVICE_UUID)
                if (gpsService != null) {
                    latitudeCharacteristic =
                        gpsService.getCharacteristic(LATITUDE_CHARACTERISTIC_UUID)
                    if (latitudeCharacteristic == null) {
                        Log.i(TAG, "latitudeCharacteristic null")
                    } else {
                        Log.i(TAG, "latitudeCharacteristic setup")
                        val gpsChar = latitudeCharacteristic!!.properties
                        Log.i(TAG, "$gpsChar")
                        val gpsCharPerm = latitudeCharacteristic!!.permissions
                        Log.i(TAG, "$gpsCharPerm")
                    }
                }

                gattLongitude = discoveredGatt
                val longService = discoveredGatt.getService(LONGITUDE_SERVICE_UUID)
                if (longService != null) {
                    longitudeCharacteristic =
                        longService.getCharacteristic(LONGITUDE_CHARACTERISTIC_UUID)
                    if (longitudeCharacteristic == null) {
                        Log.i(TAG, "longitudeCharacteristic null")
                    } else {
                        Log.i(TAG, "longitudeCharacteristic setup")
                        val gpsChar = longitudeCharacteristic!!.properties
                        Log.i(TAG, "$gpsChar")
                        val gpsCharPerm = longitudeCharacteristic!!.permissions
                        Log.i(TAG, "$gpsCharPerm")
                    }
                }

                gattNorthSouth = discoveredGatt
                val northSouthService = discoveredGatt.getService(NORTHSOUTH_SERVICE_UUID)
                if (northSouthService != null) {
                    northSouthCharacteristic =
                        northSouthService.getCharacteristic(NORTHSOUTH_CHARACTERISTIC_UUID)
                    if (northSouthCharacteristic == null) {
                        Log.i(TAG, "northSouthCharacteristic null")
                    } else {
                        Log.i(TAG, "northSouthCharacteristic setup")
                        val gpsChar = northSouthCharacteristic!!.properties
                        Log.i(TAG, "$gpsChar")
                        val gpsCharPerm = northSouthCharacteristic!!.permissions
                        Log.i(TAG, "$gpsCharPerm")
                    }
                }

                gattWestEast = discoveredGatt
                val westEastService = discoveredGatt.getService(WESTEAST_SERVICE_UUID)
                if (westEastService != null) {
                    westEastCharacteristic =
                        westEastService.getCharacteristic(WESTEAST_CHARACTERISTIC_UUID)
                    if (westEastCharacteristic == null) {
                        Log.i(TAG, "westEastCharacteristic null")
                    } else {
                        Log.i(TAG, "westEastCharacteristic setup")
                        val gpsChar = westEastCharacteristic!!.properties
                        Log.i(TAG, "$gpsChar")
                        val gpsCharPerm = westEastCharacteristic!!.permissions
                        Log.i(TAG, "$gpsCharPerm")
                    }
                }

                gattAccelCrash = discoveredGatt
                val accelCService = discoveredGatt.getService(ACCEL_CRASH_SERVICE_UUID)
                if (accelCService != null) {
                    accelCrashCharacteristic =
                        accelCService.getCharacteristic(ACCEL_CRASH_CHARACTERISTIC_UUID)
                    if (accelCrashCharacteristic == null) {
                        Log.i(TAG, "accelCrashCharacteristic null")
                    } else {
                        Log.i(TAG, "accelCrashCharacteristic setup")
                        val accCChar = accelCrashCharacteristic!!.properties
                        Log.i(TAG, "$accCChar")
                        val accCPerm = accelCrashCharacteristic!!.permissions
                        Log.i(TAG, "$accCPerm")
                    }
                }

                gattAccelAcknowledge = discoveredGatt
                val accelAService = discoveredGatt.getService(ACCEL_ACKNOWLEDGE_SERVICE_UUID)
                if (accelAService != null) {
                    accelAcknowledgeCharacteristic =
                        accelAService.getCharacteristic(ACCEL_ACKNOWLEDGE_CHARACTERISTIC_UUID)
                    if (accelAcknowledgeCharacteristic == null) {
                        Log.i(TAG, "accelAcknowledgeCharacteristic null")
                    } else {
                        Log.i(TAG, "accelAcknowledgeCharacteristic setup")
                        val accAChar = accelAcknowledgeCharacteristic!!.properties
                        Log.i(TAG, "$accAChar")
                        val accACharPerm = accelAcknowledgeCharacteristic!!.permissions
                        Log.i(TAG, "$accACharPerm")
                    }
                }

                gattTurret = discoveredGatt
                val gattService = discoveredGatt.getService(TURRET_SERVICE_UUID)
                if (gattService != null) {
                    turretCharacteristic =
                        gattService.getCharacteristic(TURRET_CHARACTERISTIC_UUID)
                    if (turretCharacteristic == null) {
                        Log.i(TAG, "turret null")
                    } else {
                        Log.i(TAG, "turret setup")
                        val accAChar = turretCharacteristic!!.properties
                        Log.i(TAG, "$accAChar")
                        val accACharPerm = turretCharacteristic!!.permissions
                        Log.i(TAG, "$accACharPerm")
                    }
                }
            }
        }
    }

    @SuppressLint("MissingPermission")
    fun startAdvertisement() {
        advertiser = adapter.bluetoothLeAdvertiser

        if (advertiseCallback == null) {
            advertiseCallback = DeviceAdvertiseCallback()

            advertiser?.startAdvertising(advertiseSettings, advertiseData, advertiseCallback)
        }
    }

    private class DeviceAdvertiseCallback : AdvertiseCallback() {
        override fun onStartFailure(errorCode: Int) {
            super.onStartFailure(errorCode)
            val errorMessage = "Advertise failed with error: $errorCode"
            Log.d(TAG, "failed $errorMessage")
        }

        override fun onStartSuccess(settingsInEffect: AdvertiseSettings) {
            super.onStartSuccess(settingsInEffect)
            Log.d(TAG, "successfully started")
        }
    }

    @SuppressLint("MissingPermission")
    fun sendForwardControl(pwm: ByteArray): Boolean {
        Log.i(TAG, "sending forward Message")
        forwardCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            Log.i(TAG, "Message byte ${pwm[0]}")
            characteristic.value = pwm
            gattForward?.let {
                val success = it.writeCharacteristic(forwardCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Forward Send message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    fun sendBackwardControl(pwm: ByteArray): Boolean {
        Log.i(TAG, "sending back Message")
        backwardCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            Log.i(TAG, "Message byte ${pwm[0]}")
            characteristic.value = pwm
            gattBackward?.let {
                val success = it.writeCharacteristic(backwardCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Backward Send message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    fun sendRightControl(pwm: ByteArray): Boolean {
        Log.i(TAG, "sending right Message")
        rightCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            Log.i(TAG, "Message byte ${pwm[0]}")
            characteristic.value = pwm
            gattRight?.let {
                val success = it.writeCharacteristic(rightCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Right Send message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    fun sendLeftControl(pwm: ByteArray): Boolean {
        Log.i(TAG, "sending left Message")
        leftCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            Log.i(TAG, "Message byte ${pwm[0]}")
            characteristic.value = pwm
            gattLeft?.let {
                val success = it.writeCharacteristic(leftCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Left Send message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    fun sendServoZeroSignal(): Boolean {
        Log.i(TAG, "sending servo zero Message")
        servoZeroCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            val pwm = byteArrayOf(10)
            pwm[0] = 50
            Log.i(TAG, "Message byte servo ${pwm[0]}")
            characteristic.value = pwm
            gattServoZero?.let {
                val success = it.writeCharacteristic(servoZeroCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Servo Zero message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    fun readLatitude(): ByteArray {
        Log.i(TAG, "reading Latitude Message")
        var arr: ByteArray = byteArrayOf()
        latitudeCharacteristic?.let { characteristic ->
            gattLatitude?.let {
                if (characteristic.value != null) {
                    arr = characteristic.value
                    val size = characteristic.value.size
                    Log.i(TAG, "size of read: $size")
                    for(i in 0 until size) {
                        Log.i(TAG, "value: ${arr[i]}")
                    }
                }
                val success = it.readCharacteristic(latitudeCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Read Latitude message was successful")
                }
            }
        }
        return arr
    }

    @SuppressLint("MissingPermission")
    fun readLongitude(): ByteArray {
        Log.i(TAG, "reading longitude Message")
        var arr: ByteArray = byteArrayOf()
        longitudeCharacteristic?.let { characteristic ->
            gattLongitude?.let {
                if (characteristic.value != null) {
                    arr = characteristic.value
                    val size = characteristic.value.size
                    Log.i(TAG, "size of read: $size")
                    for(i in 0 until size) {
                        Log.i(TAG, "value: ${arr[i]}")
                    }
                }
                val success = it.readCharacteristic(longitudeCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Read longitude message was successful")
                }
            }
        }
        return arr
    }

    @SuppressLint("MissingPermission")
    fun readNorthSouth(): ByteArray {
        Log.i(TAG, "reading northsouth Message")
        var arr: ByteArray = byteArrayOf()
        northSouthCharacteristic?.let { characteristic ->
            gattNorthSouth?.let {
                if (characteristic.value != null) {
                    arr = characteristic.value
                    val size = characteristic.value.size
                    Log.i(TAG, "size of read: $size")
                    for(i in 0 until size) {
                        Log.i(TAG, "value: ${arr[i]}")
                    }
                }
                val success = it.readCharacteristic(northSouthCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Read northsouth message was successful")
                }
            }
        }
        return arr
    }

    @SuppressLint("MissingPermission")
    fun readWestEast(): ByteArray {
        Log.i(TAG, "reading westeast Message")
        var arr: ByteArray = byteArrayOf()
        westEastCharacteristic?.let { characteristic ->
            gattWestEast?.let {
                if (characteristic.value != null) {
                    arr = characteristic.value
                    val size = characteristic.value.size
                    Log.i(TAG, "size of read: $size")
                    for(i in 0 until size) {
                        Log.i(TAG, "value: ${arr[i]}")
                    }
                }
                val success = it.readCharacteristic(westEastCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Read westeast message was successful")
                }
            }
        }
        return arr
    }

    @SuppressLint("MissingPermission")
    fun accelCrashDetected(): ByteArray {
        Log.i(TAG, "reading crash Message")
        var arr: ByteArray = byteArrayOf()
        accelCrashCharacteristic?.let { characteristic ->
//            Log.i(TAG, "characteristic not null")
            gattAccelCrash?.let {
//                Log.i(TAG, "accel crash gatt not null")
                if (characteristic.value != null) {
                    arr = characteristic.value
                    val size = characteristic.value.size
                    Log.i(TAG, "size of read accel crash: $size")
                    Log.i(TAG, "read accel crash: ${arr[0]}")
                }
                val success = it.readCharacteristic(accelCrashCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Read crash message was successful")
                }
            }
        }
        return arr
    }

    @SuppressLint("MissingPermission")
    fun accelAcknowledge(): Boolean {
        Log.i(TAG, "sending ack Message")
        accelAcknowledgeCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            val pwm = byteArrayOf(10)
            pwm[0] = 1
            Log.i(TAG, "Message byte servo ${pwm[0]}")
            characteristic.value = pwm
            gattAccelAcknowledge?.let {
                val success = it.writeCharacteristic(accelAcknowledgeCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Acknowledge message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    fun turretFire(fire: ByteArray): Boolean {
        Log.i(TAG, "sending turret Message")
        turretCharacteristic?.let { characteristic ->
            characteristic.writeType = BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE

            characteristic.value = fire
            gattTurret?.let {
                val success = it.writeCharacteristic(turretCharacteristic!!)
                if (success) {
                    Log.i(TAG, "Turret message was successful")
                    return true
                }
            }
        }
        return false
    }

    @SuppressLint("MissingPermission")
    private fun stopAdvertising() {
        Log.d(TAG, "Stopping Advertising with advertiser $advertiser")
        if (advertiseCallback != null)
            advertiser?.stopAdvertising(advertiseCallback)
        advertiseCallback = null
    }

    private fun buildAdvertiseData(): AdvertiseData {
        val dataBuilder = AdvertiseData.Builder()
            .setIncludeDeviceName(true)
            .addServiceUuid(ParcelUuid(FORWARD_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(BACKWARD_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(LEFT_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(RIGHT_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(SERVO_ZERO_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(LATITUDE_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(ACCEL_CRASH_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(ACCEL_ACKNOWLEDGE_SERVICE_UUID))
            .addServiceUuid(ParcelUuid(TURRET_SERVICE_UUID))
        return dataBuilder.build()
    }

    private fun buildAdvertiseSettings(): AdvertiseSettings {
        return AdvertiseSettings.Builder()
            .setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_LOW_LATENCY)
            .setTimeout(0)
            .build()
    }
}