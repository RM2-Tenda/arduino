<template>
    <center>
        <div id="home" class="container">
            <div class="text-right">
                <img src="./assets/logo.png" alt="RM2 Logo" class="logo">
                <button @click="logout" class="btn btn-secondary btn-sm">Log out</button>
            </div>
            <h2 class="my-4">Bem-vindo à RM2!</h2>
            <div id="map" class="map-container mb-4"></div>
        </div>
        <h3 class="mb-3">Veja também...</h3>
        <button @click="toggleModal('LED')" class="btn btn-primary">Controlo de LEDS</button>
        <button @click="toggleModal('Sensor')" class="btn btn-info">Valores dos sensores</button>
        <button @click="toggleModal('Alarm')" class="btn btn-warning">Configuração de Alarmes</button>
        <button @click="goToStatistics" class="btn btn-success">Ver Estatísticas</button>

        <!-- Modal for LED Control -->
        <div class="modal" tabindex="-1" role="dialog" v-show="showLEDPopup">
            <div class="modal-dialog" role="document">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title">LED Control</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close"
                            @click="toggleModal('LED')">
                            <span aria-hidden="true">&times;</span>
                        </button>
                    </div>
                    <div class="modal-body">
                        <div class="form-group">
                            <label for="ledPattern">LED Pattern:</label>
                            <select id="ledPattern" v-model="ledPattern" @change="setLEDPattern">
                                <option value="0">Red</option>
                                <option value="1">Green</option>
                                <option value="2">Blue</option>
                                <option value="3">Rainbow</option>
                                <option value="4">Blink</option>
                                <option value="5">Yellow Wipe</option>
                                <option value="6">White Chase</option>
                                <option value="7">Theater Chase Rainbow</option>
                                <option value="8">Color Wipe Rainbow</option>
                                <option value="9">Off</option>
                            </select>
                        </div>
                        <!-- <div v-if="ledPattern === 9" class="form-group">
                            <label for="customColor">Custom Color:</label>
                            <input type="color" id="customColor" v-model="customColor" @change="setLEDPattern"
                                class="form-control">
                        </div> -->
                    </div>
                </div>
            </div>
        </div>

        <!-- Modal for Sensor Values -->
        <div class="modal" tabindex="-1" role="dialog" v-show="showSensorPopup">
            <div class="modal-dialog" role="document">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title">Sensor Values</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close"
                            @click="toggleModal('Sensor')">
                            <span aria-hidden="true">&times;</span>
                        </button>
                    </div>
                    <div class="modal-body">
                        <ul class="list-group">
                            <li class="list-group-item">
                                Humidity: <span class="value">{{ sensorData.humidity }}%</span>
                            </li>
                            <li class="list-group-item">
                                Temperature: <span class="value">{{ sensorData.temperature }}°C</span>
                            </li>
                            <li class="list-group-item">
                                Heat Index:
                                <span class="heat-index" :class="getHeatIndexClass(sensorData.heatIndex)">
                                    {{ sensorData.heatIndex }}°C
                                </span>
                            </li>
                            <li class="list-group-item">
                                Presence: <span class="value"
                                    :class="sensorData.presence ? 'text-success' : 'text-danger'">{{ sensorData.presence
                    ? 'Detected' : 'Not Detected' }}</span>
                            </li>
                            <li class="list-group-item">
                                Gas Value: <span class="value">{{ sensorData.gasValue }} ppm</span>
                            </li>
                            <li class="list-group-item">
                                Gas Detected: <span class="value"
                                    :class="sensorData.gasDetected ? 'text-success' : 'text-danger'">{{
                    sensorData.gasDetected
                        ? 'Yes' : 'No' }}</span>
                            </li>
                            <li class="list-group-item">
                                UV Value: {{ sensorData.uvValue }}
                                <span class="uv-index" :class="getUVIndexClass(sensorData.uvValue)">
                                    {{ getUVIndex(sensorData.uvValue) }}
                                </span>
                            </li>
                        </ul>
                    </div>
                </div>
            </div>
        </div>

        <!-- Modal for Alarm Configuration -->
        <div class="modal" tabindex="-1" role="dialog" v-show="showAlarmPopup">
            <div class="modal-dialog" role="document">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title">Alarm Configuration</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close"
                            @click="toggleModal('Alarm')">
                            <span aria-hidden="true">&times;</span>
                        </button>
                    </div>
                    <div class="modal-body">
                        <div class="form-group">
                            <label for="sensor">Sensor:</label>
                            <select id="sensor" v-model="alarmConfig.sensor">
                                <option value="TEMP">Temperature</option>
                                <option value="HUM">Humidity</option>
                                <option value="GAS">Gas</option>
                                <option value="UV">UV</option>
                                <option value="MOTION">Motion</option>
                            </select>
                        </div>
                        <div class="form-group">
                            <label for="condition">Condition:</label>
                            <select id="condition" v-model="alarmConfig.condition">
                                <option value="DETECTED">Detected</option>
                                <option value="VALUE">Value</option>
                            </select>
                        </div>
                        <div class="form-group" v-if="alarmConfig.condition === 'VALUE'">
                            <label for="comparison">Comparison:</label>
                            <select id="comparison" v-model="alarmConfig.comparison">
                                <option value="ABOVE">Above</option>
                                <option value="BELOW">Below</option>
                                <option value="EQUAL">Equal</option>
                            </select>
                            <input type="number" v-model="alarmConfig.value" class="form-control">
                        </div>
                        <div class="form-group">
                            <label for="days">Days:</label>
                            <input type="text" id="days" v-model="alarmConfig.days" class="form-control"
                                placeholder="e.g., 0,1,2 for Sun, Mon, Tue">
                        </div>
                        <div class="form-group">
                            <label for="startTime">Start Time:</label>
                            <input type="time" id="startTime" v-model="alarmConfig.startTime" class="form-control">
                        </div>
                        <div class="form-group">
                            <label for="endTime">End Time:</label>
                            <input type="time" id="endTime" v-model="alarmConfig.endTime" class="form-control">
                        </div>
                        <button @click="setAlarm" class="btn btn-primary">Set Alarm</button>
                        <button @click="deleteAlarm" class="btn btn-danger">Delete Alarm</button>
                    </div>
                </div>
            </div>
        </div>
    </center>
</template>

<script>
import axios from 'axios';
import VueCookie from 'vue-cookie';

export default {
    data() {
        return {
            ledPattern: 0,
            customColor: '#ff0000',
            deviceId: 'device_001', // Hardcoded device ID
            locationPermissionGranted: false,
            map: null,
            showLEDPopup: false,
            showSensorPopup: false,
            showAlarmPopup: false,
            leds: Array(30).fill({ status: false }),
            customPattern: '',
            sensorData: {
                humidity: 0,
                temperature: 0,
                heatIndex: 0,
                presence: false,
                gasValue: 0,
                gasDetected: false,
                uvValue: 0
            },
            alarmConfig: {
                sensor: 'TEMP',
                condition: 'VALUE',
                comparison: 'ABOVE',
                value: 0,
                days: '',
                startTime: '',
                endTime: ''
            },
            alarms: [] // To store multiple alarms
        };
    },
    created() {
        this.fetchSensorData();
        this.sensorDataInterval = setInterval(this.fetchSensorData, 500);
        this.fetchAlarms();
    },
    beforeUnmount() {
        clearInterval(this.sensorDataInterval);
    },
    mounted() {
        this.checkLocationPermission();
    },
    methods: {
        async setLEDPattern() {
            let command = `SET_LED_PATTERN,${this.ledPattern}`;
            if (this.ledPattern === 9) {
                command += `,${this.customColor}`;
            }
            try {
                await axios.post('https://rm2-backend-f78fbf915aa5.herokuapp.com/api/commands', { command, device_id: this.deviceId });
            } catch (error) {
                console.error('Error setting LED pattern:', error);
                alert('Failed to set LED pattern.');
            }
        },
        getHeatIndexClass(heatIndex) {
            if (heatIndex < 27) return 'heat-index-low';
            if (heatIndex < 32) return 'heat-index-moderate';
            if (heatIndex < 41) return 'heat-index-high';
            if (heatIndex < 54) return 'heat-index-very-high';
            return 'heat-index-extreme';
        },
        getUVIndex(uvValue) {
            if (uvValue < 227) return 0;
            if (uvValue < 318) return 1;
            if (uvValue < 408) return 2;
            if (uvValue < 503) return 3;
            if (uvValue < 606) return 4;
            if (uvValue < 696) return 5;
            if (uvValue < 318) return 6;
            if (uvValue < 881) return 7;
            if (uvValue < 976) return 8;
            if (uvValue < 1079) return 9;
            if (uvValue < 1170) return 10;
            return 11;
        },
        getUVIndexClass(uvValue) {
            const uvIndex = this.getUVIndex(uvValue);
            if (uvIndex <= 2) return 'uv-low';
            if (uvIndex <= 5) return 'uv-moderate';
            if (uvIndex <= 7) return 'uv-high';
            if (uvIndex <= 10) return 'uv-very-high';
            return 'uv-extreme';
        },
        checkLocationPermission() {
            const permissionCookie = VueCookie.get('locationPermission');
            if (permissionCookie === 'granted') {
                this.locationPermissionGranted = true;
                navigator.geolocation.getCurrentPosition(
                    (position) => {
                        this.initMap(position);
                    },
                    (error) => {
                        console.error('Error getting location:', error);
                    }
                );
            } else {
                this.requestLocationPermission();
            }
        },
        requestLocationPermission() {
            if (navigator.geolocation) {
                navigator.geolocation.getCurrentPosition(
                    (position) => {
                        this.locationPermissionGranted = true;
                        VueCookie.set('locationPermission', 'granted', { expires: '1Y' });
                        this.initMap(position);
                    },
                    (error) => {
                        console.error('Error getting location:', error);
                        VueCookie.set('locationPermission', 'denied', { expires: '1Y' });
                    }
                );
            } else {
                console.error('Geolocation is not supported by this browser.');
            }
        },
        initMap(position) {
            const pos = {
                lat: position.coords.latitude,
                lng: position.coords.longitude
            };

            this.map = new google.maps.Map(document.getElementById('map'), {
                center: pos,
                zoom: 15
            });

            this.currentMarker = new google.maps.Marker({
                position: pos,
                map: this.map,
                title: 'Your location'
            });

            this.updateDeviceMarker();
        },
        updateDeviceMarker() {
            if (this.sensorData.latitude && this.sensorData.longitude) {
                if (this.deviceMarker) {
                    this.deviceMarker.setPosition({ lat: this.sensorData.latitude, lng: this.sensorData.longitude });
                } else {
                    this.deviceMarker = new google.maps.Marker({
                        position: { lat: this.sensorData.latitude, lng: this.sensorData.longitude },
                        map: this.map,
                        title: 'Tenda'
                    });
                }
            }
        },
        handleLocationError(browserHasGeolocation, map, pos) {
            alert(browserHasGeolocation ?
                'Error: The Geolocation service failed.' :
                'Error: Your browser doesn\'t support geolocation.');
            if (map) map.setCenter(pos);
        },
        logout() {
            this.$store.commit('clearAuthToken');
            this.$router.push({ name: 'login' });
        },
        toggleModal(type) {
            if (type === 'LED') {
                this.showLEDPopup = !this.showLEDPopup;
            } else if (type === 'Sensor') {
                this.showSensorPopup = !this.showSensorPopup;
            } else if (type === 'Alarm') {
                this.showAlarmPopup = !this.showAlarmPopup;
            }
        },
        async setLED(index) {
            const ledState = this.leds[index].status;
            try {
                await axios.post('https://rm2-backend-f78fbf915aa5.herokuapp.com/api/button/set', { index: index, result: ledState });
            } catch (error) {
                console.error('Error updating LED state:', error);
                this.leds[index].status = !ledState;
                alert('Failed to update LED state.');
            }
        },
        async setAlarm() {
            const { sensor, condition, comparison, value, days, startTime, endTime } = this.alarmConfig;
            try {
                await axios.post('https://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms', {
                    sensor, condition, comparison, value, days, startTime, endTime, device_id: this.deviceId
                });
                alert('Alarm set successfully.');
                this.fetchAlarms();
            } catch (error) {
                console.error('Error setting alarm:', error);
                alert('Failed to set alarm.');
            }
        },
        async deleteAlarm() {
            try {
                await axios.delete(`https://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms/${this.alarmConfig.id}`);
                alert('Alarm deleted successfully.');
                this.fetchAlarms();
            } catch (error) {
                console.error('Error deleting alarm:', error);
                alert('Failed to delete alarm.');
            }
        },
        async fetchSensorData() {
            try {
                const response = await axios.get(`https://rm2-backend-f78fbf915aa5.herokuapp.com/api/statistics?device_id=${this.deviceId}`, {
                    headers: { Authorization: `Bearer ${this.$store.state.authToken}` }
                });
                const latestData = response.data;
                const c1 = -8.78469475556;
                const c2 = 1.61139411;
                const c3 = 2.33854883889;
                const c4 = -0.14611605;
                const c5 = -0.012308094;
                const c6 = -0.0164248277778;
                const c7 = 0.002211732;
                const c8 = 0.00072546;
                const c9 = -0.000003582;

                if (latestData) {
                    this.sensorData.humidity = parseFloat(latestData.humidity);
                    this.sensorData.temperature = parseFloat(latestData.temperature);
                    this.sensorData.presence = latestData.presence;
                    this.sensorData.gasValue = parseInt(latestData.gas_value);
                    this.sensorData.gasDetected = latestData.gas_detected;
                    this.sensorData.uvValue = parseInt(latestData.uv_value);

                    // Calculate heat index
                    const T = parseFloat(latestData.temperature);
                    const RH = parseFloat(latestData.humidity) / 100;

                    const heatIndex = c1 +
                        c2 * T +
                        c3 * RH +
                        c4 * T * RH +
                        c5 * T * T +
                        c6 * RH * RH +
                        c7 * T * T * RH +
                        c8 * T * RH * RH +
                        c9 * T * T * RH * RH;

                    this.sensorData.heatIndex = heatIndex.toFixed(2);

                    // Handle GPS data
                    if (!isNaN(latestData.latitude) && !isNaN(latestData.longitude)) {
                        this.sensorData.latitude = latestData.latitude;
                        this.sensorData.longitude = latestData.longitude;
                    }
                }

            } catch (error) {
                console.error('Error fetching sensor data:', error);
            }
        },
        async fetchAlarms() {
            try {
                const response = await axios.get(`https://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms?device_id=${this.deviceId}`);
                this.alarms = response.data.alarms;
            } catch (error) {
                console.error('Error fetching alarms:', error);
            }
        },
        goToStatistics() {
            this.$router.push({ name: 'statistics', params: { deviceId: this.deviceId } });
        }
    }
}
</script>

<style scoped>
@import url('https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css');

.uv-index {
    margin-left: 5px;
    padding: 2px 5px;
    border-radius: 4px;
    color: white;
}

.uv-low {
    background-color: #289500;
}

.uv-moderate {
    background-color: #f7e400;
}

.uv-high {
    background-color: #f85900;
}

.uv-very-high {
    background-color: #d8001d;
}

.uv-extreme {
    background-color: #6b49c8;
}

.fa-spin {
    animation: fa-spin 2s infinite linear;
}

@keyframes fa-spin {
    0% {
        transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
    }
}

.map-container {
    height: 400px;
    width: 100%;
}

.map-container img {
    max-width: 100%;
    height: auto;
    border-radius: 0.25rem;
}

.logo {
    height: 50px;
}

.modal {
    display: block;
    background-color: rgba(0, 0, 0, 0.5);
    position: fixed;
    left: 0;
    top: 0;
    width: 100%;
    height: 100%;
    overflow: auto;
    padding-top: 100px;
}

.modal-content {
    background-color: #fefefe;
    margin: auto;
    padding: 20px;
    border: 1px solid #888;
    width: 80%;
}

.close {
    color: #aaa;
    float: right;
    font-size: 28px;
    font-weight: bold;
}

.close:hover,
.close:focus {
    color: black;
    text-decoration: none;
    cursor: pointer;
}

.led-control {
    display: flex;
    align-items: center;
    justify-content: start;
    margin-bottom: 1rem;
}

.led-light {
    display: inline-block;
    width: 24px;
    height: 24px;
    border-radius: 50%;
    margin-left: 15px;
    background-color: #ccc;
}

.led-light-on {
    background-color: #ff0;
}

.led-light-off {
    background-color: #888;
}

.list-group-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.text-success {
    color: #28a745;
}

.text-danger {
    color: #dc3545;
}

.heat-index {
    float: right;
    padding: 2px 5px;
    border-radius: 4px;
    color: white;
}

.heat-index-low {
    background-color: #28a745;
}

.heat-index-moderate {
    background-color: #ffc107;
}

.heat-index-high {
    background-color: #fd7e14;
}

.heat-index-very-high {
    background-color: #dc3545;
}

.heat-index-extreme {
    background-color: #6c757d;
}

.value {
    float: right;
    font-weight: bold;
}

.text-success {
    color: #28a745;
}

.text-danger {
    color: #dc3545;
}
</style>