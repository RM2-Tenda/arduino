<template>
    <div id="home" class="container">
        <div class="header">
            <img src="./assets/logo.png" alt="RM2 Logo" class="logo">
            <button @click="logout" class="btn btn-secondary btn-sm logout-btn">Log out</button>
        </div>
        <h2 class="my-4">Bem-vindo à RM2!</h2>
        <div class="row">
            <div class="col-md-8">
                <div id="map" class="map-container mb-4"></div>
                <div id="directionsPanel" class="directions-panel" v-show="showDirectionsPanel"></div>
            </div>
            <div class="col-md-4">
                <div class="card mb-4">
                    <div class="card-body">
                        <h5 class="card-title">Navegação</h5>
                        <button v-if="!routeCreated" @click="createRoute" class="btn btn-primary btn-block mb-2">Criar
                            Rota até Tenda</button>
                        <button v-if="routeCreated && !navigationStarted" @click="deleteRoute"
                            class="btn btn-danger btn-block mb-2">Apagar Rota</button>
                        <div v-if="routeInfo" class="route-info">
                            <h4>Route Information</h4>
                            <p><strong>Distance:</strong> {{ routeInfo.distance }}</p>
                            <p><strong>Duration:</strong> {{ routeInfo.duration }}</p>
                        </div>
                        <button v-if="routeCreated && !navigationStarted" @click="startNavigation"
                            class="btn btn-success btn-block mb-2">Iniciar Navegação</button>
                        <button v-if="navigationStarted" @click="stopNavigation" class="btn btn-danger btn-block">Parar
                            Navegação</button>
                    </div>
                </div>
                <div class="card mb-4">
                    <div class="card-body">
                        <h5 class="card-title">Controles do Mapa</h5>
                        <button @click="centerMap" class="btn btn-secondary btn-block mb-2">Centrar Mapa</button>
                        <button @click="toggleDirectionsPanel" class="btn btn-info btn-block">Mostrar/Ocultar
                            Direções</button>
                    </div>
                </div>
                <div class="card">
                    <div class="card-body">
                        <h5 class="card-title">Ações</h5>
                        <button @click="toggleModal('LED')" class="btn btn-primary btn-block mb-2">Controlo de
                            LEDS</button>
                        <button @click="toggleModal('Sensor')" class="btn btn-info btn-block mb-2">Valores dos
                            sensores</button>
                        <button @click="toggleModal('Alarm')" class="btn btn-warning btn-block mb-2">Criar
                            Alarmes</button>
                        <button @click="goToAlarms" class="btn btn-danger btn-block">Gerir Alarms</button>
                        <button @click="goToStatistics" class="btn btn-success btn-block">Ver Estatísticas</button>
                    </div>
                </div>
            </div>
        </div>
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
        <div class="modal alarm-modal" tabindex="-1" role="dialog" v-show="showAlarmPopup">
            <div class="modal-dialog" role="document">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title">Configure Alarm</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close"
                            @click="toggleModal('Alarm')">
                            <span aria-hidden="true">&times;</span>
                        </button>
                    </div>
                    <form @submit.prevent="submitAlarm">
                        <div class="modal-body">
                            <div class="form-group">
                                <label for="sensor">Sensor:</label>
                                <select id="sensor" v-model="alarmConfig.sensor" class="form-control">
                                    <option value="TEMP">Temperature</option>
                                    <option value="HUMID">Humidity</option>
                                    <option value="GAS">Gas</option>
                                    <option value="UV">UV</option>
                                </select>
                            </div>
                            <div class="form-group">
                                <label for="condition">Condition:</label>
                                <select id="condition" v-model="alarmConfig.condition" class="form-control">
                                    <option value="VALUE">Value</option>
                                    <option value="MOTION">Presence</option>
                                </select>
                            </div>
                            <div class="form-group">
                                <label for="comparison">Comparison:</label>
                                <select id="comparison" v-model="alarmConfig.comparison" class="form-control">
                                    <option value="ABOVE">Above</option>
                                    <option value="BELOW">Below</option>
                                </select>
                            </div>
                            <div class="form-group">
                                <label for="value">Value:</label>
                                <input type="number" id="value" v-model="alarmConfig.value" class="form-control">
                            </div>
                            <div>
                                <label for="days">Days:</label>
                                <select id="days" v-model="alarmDays" multiple>
                                    <option value="Mon">Monday</option>
                                    <option value="Tue">Tuesday</option>
                                    <option value="Wed">Wednesday</option>
                                    <option value="Thu">Thursday</option>
                                    <option value="Fri">Friday</option>
                                    <option value="Sat">Saturday</option>
                                    <option value="Sun">Sunday</option>
                                </select>
                            </div>
                            <div>
                                <label for="startTime">Start Time:</label>
                                <input type="time" id="startTime" v-model="alarmStartTime" required>
                            </div>
                            <div>
                                <label for="endTime">End Time:</label>
                                <input type="time" id="endTime" v-model="alarmEndTime" required>
                            </div>
                        </div>
                    </form>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-primary" @click="setAlarm">Set Alarm</button>
                        <button type="button" class="btn btn-secondary" @click="toggleModal('Alarm')">Close</button>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import axios from 'axios';
import VueCookie from 'vue-cookie';
import { debounce } from 'lodash';

export default {
    data() {
        return {
            googleMapsScriptLoaded: false,
            routeCreated: false,
            navigationStarted: false,
            navigationInterval: null,
            proximityCheckInterval: null,
            showDirectionsPanel: false,
            ledPattern: 0,
            customColor: '#ff0000',
            deviceId: 'device_001',
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
            alarms: [], // To store multiple alarms
            directionsRenderer: null, // To store the directions renderer
            currentMarker: null,
            deviceMarker: null,
            routeInfo: null,
            alarmDays: [],
            alarmStartTime: '',
            alarmEndTime: '',
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
    async mounted() {
        try {
            await this.loadGoogleMapsScript();
            navigator.geolocation.getCurrentPosition(
                async position => {
                    await this.initMap(position);
                },
                error => {
                    console.error('Geolocation error:', error);
                }
            );
            this.checkLocationPermission();
        } catch (error) {
            console.error('Error loading Google Maps:', error);
        }
    },
methods: {
    async loadGoogleMapsScript() {
        if (this.googleMapsScriptLoaded) {
            return;
        }
        if (typeof google !== 'undefined' && google.maps) {
            this.googleMapsScriptLoaded = true;
            return;
        }
        return new Promise((resolve, reject) => {
            const script = document.createElement('script');
            script.src = `https://maps.googleapis.com/maps/api/js?language=pt-PT&key=AIzaSyDWz1Zm2QIrI_eTykZpcN5baQgJQeJouIc&loading=async`;
            script.async = true;
            script.defer = true;
            script.onload = () => {
                this.googleMapsScriptLoaded = true;
                resolve();
            };
            script.onerror = reject;
            document.head.appendChild(script);
        });
    },
    async initMap(position) {
        await this.loadGoogleMapsScript();
        if (!this.googleMapsScriptLoaded) {
            console.error("Google Maps script not loaded.");
            return;
        }
        const pos = {
            lat: position.coords.latitude,
            lng: position.coords.longitude
        };
        const tendaPos = {
            lat: pos.lat + 0.009, // Approximately 1 km north
            lng: pos.lng + 0.009  // Approximately 1 km east
        };

        this.map = new google.maps.Map(document.getElementById('map'), {
            center: pos,
            zoom: 18
        });

        this.currentMarker = new google.maps.Marker({
            position: pos,
            map: this.map,
            title: 'Your location',
            icon: 'https://maps.google.com/mapfiles/kml/shapes/man.png' // Person icon
        });

        this.deviceMarker = new google.maps.Marker({
            position: tendaPos,
            map: this.map,
            title: 'Tenda',
            icon: 'http://maps.google.com/mapfiles/kml/shapes/campground.png' // Tent icon
        });
    },
    goToAlarms() {
        this.$router.push({ name: 'alarms', params: { deviceId: this.deviceId } });
    },
    createRoute() {
        if (!this.locationPermissionGranted || !this.map) {
            alert('Location permission not granted or map not initialized.');
            return;
        }

        const currentPos = this.currentMarker.getPosition().toJSON();
        const tendaPos = this.deviceMarker.getPosition().toJSON();

        const directionsService = new google.maps.DirectionsService();
        this.directionsRenderer = new google.maps.DirectionsRenderer({
            map: this.map,
            panel: document.getElementById('directionsPanel')
        });

        const request = {
            origin: currentPos,
            destination: tendaPos,
            travelMode: 'WALKING'
        };

        directionsService.route(request, (result, status) => {
            if (status === 'OK') {
                this.directionsRenderer.setDirections(result);
                this.updateRouteInfo(result);
            } else {
                console.error('Error creating route:', status);
                alert('Failed to create route.');
            }
        });
        this.routeCreated = true;
    },
    deleteRoute() {
        if (this.directionsRenderer) {
            this.directionsRenderer.setMap(null);
            this.directionsRenderer = null;
        }
        this.routeCreated = false;
        this.routeInfo = null;
    },
        async centerMap() {
        const currentPos = this.currentMarker.getPosition().toJSON();
        this.map.setCenter(currentPos);
        if (this.navigationStarted) {
            this.map.setZoom(20);
            this.map.setTilt(60);
        }
    },
    startNavigation() {
        if (!this.locationPermissionGranted || !this.map) {
            alert('Location permission not granted or map not initialized.');
            return;
        }

        const currentPos = this.currentMarker.getPosition().toJSON();
        const tendaPos = this.deviceMarker.getPosition().toJSON();
        this.map.setCenter(currentPos); // Center the map on the current position
        // Set zoom and tilt for navigation mode
        this.map.setZoom(20); // Increase zoom level for an even closer view
        this.map.setTilt(60); // Increase tilt angle for a more immersive 3D view

        const directionsService = new google.maps.DirectionsService();

        const request = {
            origin: currentPos,
            destination: tendaPos,
            travelMode: 'WALKING'
        };

        directionsService.route(request, (result, status) => {
            if (status === 'OK') {
                this.updateRoutePolyline(result.routes[0].overview_path);
                this.provideRealTimeVoiceDirections(result);
                this.updateMapHeading(currentPos, tendaPos);
                this.updateRouteInfo(result);
            } else {
                console.error('Error creating route:', status);
                alert('Failed to create route.');
            }
        });

        this.navigationInterval = setInterval(() => {
            navigator.geolocation.getCurrentPosition(
                (position) => {
                    const pos = {
                        lat: position.coords.latitude,
                        lng: position.coords.longitude
                    };
                    this.currentMarker.setPosition(pos);

                    // Get the device orientation
                    if (window.DeviceOrientationEvent) {
                        window.addEventListener('deviceorientation', (event) => {
                            const heading = event.alpha; // Get the device's heading in degrees
                            this.map.setHeading(heading); // Set the map's heading to match the device's heading
                        });
                    }

                    // Update directions only if moved significantly
                    const distance = google.maps.geometry.spherical.computeDistanceBetween(
                        this.currentMarker.getPosition(),
                        pos
                    );

                    if (distance > 50) { // Update if moved more than 50 meters
                        const newRequest = {
                            origin: pos,
                            destination: tendaPos,
                            travelMode: 'WALKING'
                        };

                        directionsService.route(newRequest, (result, status) => {
                            if (status === 'OK') {
                                this.updateRoutePolyline(result.routes[0].overview_path);
                                this.provideRealTimeVoiceDirections(result);
                                this.updateRouteInfo(result);
                            } else {
                                console.error('Error updating route:', status);
                            }
                        });
                    }
                },
                (error) => {
                    console.error('Error getting location:', error);
                }
            );
        }, 5000); // Update every 5 seconds
        this.navigationStarted = true;
    },
    stopNavigation() {
        if (this.navigationInterval) {
            clearInterval(this.navigationInterval);
            this.navigationInterval = null;
        }
        if (this.proximityCheckInterval) {
            clearInterval(this.proximityCheckInterval);
            this.proximityCheckInterval = null;
        }
        if (this.directionsRenderer) {
            this.directionsRenderer.setMap(null);
            this.directionsRenderer = null;
        }
        if (this.routePolyline) {
            this.routePolyline.setMap(null);
            this.routePolyline = null;
        }
        this.navigationStarted = false;
        this.routeCreated = false;
        this.routeInfo = null;
        this.map.setCenter(this.currentMarker.getPosition());
        this.map.setZoom(18);
        this.map.setTilt(0);
        this.map.setHeading(0);
        alert('Navegação encerrada.');
    },
    updateRouteInfo(result) {
        const duration = result.routes[0].legs[0].duration.text;
        const distance = result.routes[0].legs[0].distance.text;
        this.routeInfo = {
            distance,
            duration
        };
    },
    updateRoutePolyline(path) {
        if (this.routePolyline) {
            this.routePolyline.setMap(null);
        }

        this.routePolyline = new google.maps.Polyline({
            path: path,
            geodesic: true,
            strokeColor: '#FF0000',
            strokeOpacity: 1.0,
            strokeWeight: 2
        });

        this.routePolyline.setMap(this.map);
    },
    updateMapHeading: debounce(function (start, end) {
        const heading = this.getHeading(start, end);
        if (this.map.getHeading() !== heading) {
            this.map.setHeading(heading);
        }
    }, 1000),
        provideRealTimeVoiceDirections(result) {
        const speechSynthesis = window.speechSynthesis;
        const route = result.routes[0];
        const steps = route.legs[0].steps;

        // Clear any existing speech queue
        speechSynthesis.cancel();

        let currentStepIndex = 0;

        const checkProximity = () => {
            if (currentStepIndex >= steps.length) {
                clearInterval(this.proximityCheckInterval);
                return;
            }

            const step = steps[currentStepIndex];
            const stepPosition = step.end_location;
            const currentPosition = this.currentMarker.getPosition();

            const distance = google.maps.geometry.spherical.computeDistanceBetween(currentPosition, stepPosition);

            if (distance < 50) { // 50 meters proximity
                const instructions = this.stripHtmlTags(step.instructions);
                const msg = new SpeechSynthesisUtterance(instructions);
                msg.lang = 'pt-PT';
                msg.rate = 1;
                msg.pitch = 1;
                msg.volume = 1;
                speechSynthesis.speak(msg);
                currentStepIndex++;
            }
        };

        this.proximityCheckInterval = setInterval(checkProximity, 1000); // Check every second
    },

    stripHtmlTags(html) {
        const div = document.createElement('div');
        div.innerHTML = html;
        return div.textContent || div.innerText || '';
    },
    getHeading(start, end) {
        const lat1 = start.lat;
        const lng1 = start.lng;
        const lat2 = end.lat;
        const lng2 = end.lng;

        const dLng = lng2 - lng1;
        const y = Math.sin(dLng) * Math.cos(lat2);
        const x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(dLng);
        const brng = Math.atan2(y, x) * (180 / Math.PI);
        return (brng + 360) % 360; // Convert to 0-360 range
    },
    exitDirectionsMode() {
        if (this.navigationInterval) {
            clearInterval(this.navigationInterval);
            this.navigationInterval = null;
        }
        if (this.proximityCheckInterval) {
            clearInterval(this.proximityCheckInterval);
            this.proximityCheckInterval = null;
        }
        if (this.directionsRenderer) {
            this.directionsRenderer.setMap(null);
            this.directionsRenderer = null;
        }
        if (this.routePolyline) {
            this.routePolyline.setMap(null);
            this.routePolyline = null;
        }
        this.showDirectionsPanel = false;
        this.routeInfo = null; // Clear route information
        this.map.setCenter(this.currentMarker.getPosition()); // Reset map center
        this.map.setZoom(18); // Reset zoom level
        this.map.setTilt(0); // Reset tilt
        this.map.setHeading(0); // Reset heading
        alert('Modo de direções encerrado.');
    },
    toggleDirectionsPanel() {
        this.showDirectionsPanel = !this.showDirectionsPanel;
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

.container {
    padding: 20px;
}

.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}

.logo {
    height: 50px;
}

.btn {
    margin-bottom: 10px;
}

.card {
    margin-bottom: 20px;
}

.card-title {
    font-size: 1.25rem;
    margin-bottom: 15px;
}

.card-body {
    padding: 20px;
}

.map-container {
    height: 400px;
    width: 100%;
    border: 1px solid #ccc;
    border-radius: 8px;
    overflow: hidden;
}

.directions-panel {
    height: 200px;
    overflow-y: auto;
    border: 1px solid #ccc;
    border-radius: 8px;
    padding: 10px;
    margin-top: 10px;
}

.route-info {
    background-color: #f8f9fa;
    border: 1px solid #ccc;
    border-radius: 8px;
    padding: 15px;
    margin-top: 10px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
}

.route-info h4 {
    margin-bottom: 10px;
    font-size: 1.5rem;
    font-weight: bold;
}

.route-info p {
    margin: 5px 0;
    font-size: 1.1rem;
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

.alarm-modal .modal-content {
    border-radius: 10px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
}

.alarm-modal .modal-header {
    background-color: #f8f9fa;
    border-bottom: none;
}

.alarm-modal .modal-title {
    font-size: 1.5rem;
    font-weight: bold;
}

.alarm-modal .form-control {
    border-radius: 5px;
}

.alarm-modal .btn-primary {
    background-color: #007bff;
    border-color: #007bff;
}

.alarm-modal .btn-primary:hover {
    background-color: #0069d9;
    border-color: #0062cc;
}

.alarm-modal .btn-secondary {
    background-color: #6c757d;
    border-color: #6c757d;
}

.alarm-modal .btn-secondary:hover {
    background-color: #5a6268;
    border-color: #545b62;
}
</style>