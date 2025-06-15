package com.example.weatherdashboard

import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import java.time.LocalDateTime
import java.time.OffsetDateTime
import java.time.format.DateTimeFormatter

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        try {
            setContent {
                DashboardScreen()
            }
        } catch (e: Exception) {
            e.printStackTrace()
            // Możesz też dodać Log.e dla lepszej widoczności
            android.util.Log.e("MainActivity", "Exception in setContent", e)
        }
    }
}

@Composable
fun DashboardScreen(viewModel: MainViewModel = viewModel()) {
    val sensorData by viewModel.sensorData.collectAsState()
    val error by viewModel.error.collectAsState()

    MaterialTheme {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp),
            verticalArrangement = Arrangement.SpaceEvenly,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text("Weather Dashboard", style = MaterialTheme.typography.headlineMedium)

            Spacer(modifier = Modifier.height(16.dp))

            if (error != null) {
                Text(text = error ?: "", color = Color.Red)
                Spacer(modifier = Modifier.height(8.dp))
            }

            sensorData?.let { data ->
                InfoCard(title = "Temperature", value = "${data.temperature} °C")
                InfoCard(title = "Humidity", value = "${data.humidity} %")
                InfoCard(title = "Pressure", value = "${data.pressure} hPa")

                InfoCard(title = "Ostatnia aktualizacja", value = shortenTimestamp(data.timestamp))

            } ?: run {
                CircularProgressIndicator()
            }

            Spacer(modifier = Modifier.height(24.dp))

            Button(onClick = { viewModel.refreshNow() }) {
                Text("Odśwież teraz")
            }
        }
    }
}

fun shortenTimestamp(timestamp: String): String {
    return if (timestamp.length >= 16) {
        timestamp.substring(0, 16).replace('T', ' ')
    } else {
        timestamp.replace('T', ' ')
    }
}




@Composable
fun InfoCard(title: String, value: String) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(8.dp),
        shape = RoundedCornerShape(16.dp),
        elevation = CardDefaults.cardElevation(defaultElevation = 8.dp)
    ) {
        Column(
            modifier = Modifier.padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(text = title, style = MaterialTheme.typography.titleMedium)
            Text(text = value, style = MaterialTheme.typography.headlineSmall)
        }
    }
}