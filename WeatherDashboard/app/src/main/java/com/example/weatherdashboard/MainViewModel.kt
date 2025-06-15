package com.example.weatherdashboard

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class MainViewModel : ViewModel() {
    private val _sensorData = MutableStateFlow<SensorData?>(null)
    val sensorData = _sensorData.asStateFlow()

    private val _error = MutableStateFlow<String?>(null)
    val error = _error.asStateFlow()

    init {
        refreshDataPeriodically()
    }

    private fun refreshDataPeriodically() {
        viewModelScope.launch {
            while (true) {
                refreshNow()
                delay(60_000L)
            }
        }
    }


    fun refreshNow() {
        viewModelScope.launch {
            try {
                val latest = SupabaseApi.fetchLatestSensorData()
                _sensorData.value = latest
                _error.value = null
            } catch (e: Exception) {
                e.printStackTrace()
                _error.value = "Błąd pobierania danych: ${e.message}"
            }
        }
    }
}