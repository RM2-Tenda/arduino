<template>
    <div class="container">
        <div class="header">
            <button @click="goBack" class="btn btn-secondary btn-sm back-btn">
                <i class="fas fa-arrow-left"></i> Back
            </button>
            <h2 class="my-4">Manage Alarms</h2>
            <button @click="logout" class="btn btn-secondary btn-sm logout-btn">Log out</button>
        </div>
        <table class="table table-striped">
            <thead>
                <tr>
                    <th>Sensor</th>
                    <th>Condition</th>
                    <th>Value</th>
                    <th>Days</th>
                    <th>Start Time</th>
                    <th>End Time</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                <tr v-for="alarm in alarms" :key="alarm.id">
                    <td>{{ alarm.sensor }}</td>
                    <td>{{ alarm.condition }}</td>
                    <td>{{ alarm.value }}</td>
                    <td>{{ alarm.days }}</td>
                    <td>{{ alarm.start_time }}</td>
                    <td>{{ alarm.end_time }}</td>
                    <td>
                        <button @click="deleteAlarm(alarm.id)" class="btn btn-danger btn-sm">Delete</button>
                    </td>
                </tr>
            </tbody>
        </table>
        <button @click="clearAlarms" class="btn btn-danger btn-block mt-4">Clear All Alarms</button>
    </div>
</template>

<script>
import axios from 'axios';

export default {
    data() {
        return {
            alarms: [],
            deviceId: this.$route.params.deviceId
        };
    },
    created() {
        this.fetchAlarms();
    },
    methods: {
        async fetchAlarms() {
            try {
                const response = await axios.get(`https://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms?device_id=${this.deviceId}`);
                this.alarms = response.data.alarms;
            } catch (error) {
                console.error('Error fetching alarms:', error);
            }
        },
        async deleteAlarm(alarmId) {
            try {
                await axios.delete(`https://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms/${alarmId}`);
                this.fetchAlarms();
            } catch (error) {
                console.error('Error deleting alarm:', error);
            }
        },
        async clearAlarms() {
            try {
                await axios.post('https://rm2-backend-f78fbf915aa5.herokuapp.com/api/alarms/clear', { device_id: this.deviceId });
                this.fetchAlarms();
            } catch (error) {
                console.error('Error clearing alarms:', error);
            }
        },
        goBack() {
            this.$router.push({ name: 'home', params: { deviceId: this.deviceId } });
        },
        logout() {
            this.$store.commit('clearAuthToken');
            this.$router.push({ name: 'login' });
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

.back-btn {
    margin-right: auto;
}

.logout-btn {
    margin-left: auto;
}

.table {
    margin-top: 20px;
}

.btn-block {
    width: 100%;
}
</style>