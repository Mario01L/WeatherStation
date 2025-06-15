package com.example.weatherdashboard

import io.ktor.client.*
import io.ktor.client.call.*
import io.ktor.client.engine.okhttp.*
import io.ktor.client.plugins.contentnegotiation.*
import io.ktor.client.request.*
import io.ktor.http.*
import io.ktor.serialization.kotlinx.json.*
import kotlinx.serialization.json.Json

object SupabaseApi {

    private const val SUPABASE_URL = "https://encglewfzpvfboxsprqs.supabase.co"
    private const val SUPABASE_KEY = "Supabase_KEY"

    private val client = HttpClient(OkHttp) {
        install(ContentNegotiation) {
            json(Json {
                ignoreUnknownKeys = true
            })
        }
    }

    suspend fun fetchLatestSensorData(): SensorData {
        val url = "$SUPABASE_URL/rest/v1/dane?select=*&order=timestamp.desc&limit=1"

        try {
            val response = client.get(url) {
                headers {
                    append("apikey", SUPABASE_KEY)
                    append("Authorization", "Bearer $SUPABASE_KEY")
                    append(HttpHeaders.Accept, "application/json")
                }
            }

            if (!response.status.isSuccess()) {
                throw Exception("HTTP ${response.status.value}: ${response.status.description}")
            }

            val data: List<SensorData> = response.body()
            return data.firstOrNull() ?: throw Exception("Brak danych w Supabase")

        } catch (e: Exception) {
            throw Exception("Błąd Supabase: ${e.message}", e)
        }
    }
}
