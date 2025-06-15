package com.example.weatherdashboard

import kotlinx.serialization.Serializable

@Serializable
data class SensorData(
    val id: Int,
    val temperature: Float,
    val humidity: Float,
    val pressure: Float,
    val timestamp: String
)