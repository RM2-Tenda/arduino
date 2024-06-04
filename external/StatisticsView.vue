<template>
    <center>
        <div id="statistics" class="container">
            <div class="text-right">
                <button @click="goHome" class="btn btn-secondary btn-sm">Home</button>
            </div>
            <h2 class="my-4">Estatísticas dos Sensores</h2>
            <div class="form-group">
                <label for="timeRange">Intervalo de Tempo:</label>
                <select id="timeRange" v-model="timeRange" @change="fetchStatistics">
                    <option value="day">Último Dia</option>
                    <option value="week">Última Semana</option>
                    <option value="month">Último Mês</option>
                </select>
            </div>
            <div id="charts">
                <canvas id="humidityChart"></canvas>
                <canvas id="temperatureChart"></canvas>
                <canvas id="presenceChart"></canvas>
                <canvas id="gasChart"></canvas>
                <canvas id="uvChart"></canvas>
            </div>
        </div>
    </center>
</template>

<script>
import axios from 'axios';
import Chart from 'chart.js';

export default {
    props: ['deviceId'],
    data() {
        return {
            timeRange: 'day',
            statistics: []
        };
    },
    created() {
        this.fetchStatistics();
    },
    methods: {
        goHome() {
            this.$router.push({ name: 'home' });
        },
        async fetchStatistics() {
            try {
                const response = await axios.get(`https://rm2-backend-f78fbf915aa5.herokuapp.com/api/statistics/history?device_id=${this.deviceId}`);
                this.statistics = response.data.statistics;
                this.updateCharts();
            } catch (error) {
                console.error('Error fetching statistics:', error);
            }
        },
        updateCharts() {
            const labels = this.statistics.map(stat => new Date(stat.timestamp).toLocaleString());
            const humidityData = this.statistics.map(stat => stat.humidity);
            const temperatureData = this.statistics.map(stat => stat.temperature);
            const presenceData = this.statistics.map(stat => stat.presence ? 1 : 0);
            const gasData = this.statistics.map(stat => stat.gas_value);
            const uvData = this.statistics.map(stat => stat.uv_value);

            this.createChart('humidityChart', 'Humidity (%)', labels, humidityData);
            this.createChart('temperatureChart', 'Temperature (°C)', labels, temperatureData);
            this.createChart('presenceChart', 'Presence', labels, presenceData);
            this.createChart('gasChart', 'Gas Value (ppm)', labels, gasData);
            this.createChart('uvChart', 'UV Value', labels, uvData);
        },
        createChart(chartId, label, labels, data) {
            const ctx = document.getElementById(chartId).getContext('2d');
            new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: label,
                        data: data,
                        borderColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                },
                options: {
                    scales: {
                        xAxes: [{
                            type: 'time',
                            time: {
                                unit: this.timeRange === 'day' ? 'hour' : this.timeRange === 'week' ? 'day' : 'month'
                            }
                        }],
                        yAxes: [{
                            ticks: {
                                beginAtZero: true
                            }
                        }]
                    }
                }
            });
        }
    }
}
</script>

<style scoped>
.container {
    margin-top: 20px;
}

#charts {
    display: flex;
    flex-direction: column;
    align-items: center;
}

canvas {
    max-width: 100%;
    margin-bottom: 20px;
}
</style>