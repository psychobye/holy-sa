package com.holy.game.gui

import android.os.CountDownTimer
import android.view.View
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.TaxiBinding
import com.rommansabbir.animationx.Attention
import com.rommansabbir.animationx.animationXAttention

class Taxi : NativeGui<TaxiBinding>(TaxiBinding::class) {
    private val durationInMillis = 10_000L
    private lateinit var acceptOrderTimer: CountDownTimer

    private external fun nativeAcceptOrder()

    init {
        activity.runOnUiThread {

            binding.toggleButt.setOnClickListener {
                val toggle = binding.windowLayout.visibility == View.GONE
                toggleWindow(toggle)
            }


            binding.acceptOrderClicableArea.setOnClickListener    {
                nativeAcceptOrder()
                acceptOrderTimer.cancel()
                binding.newOrderLayout.visibility = View.GONE
            }

            acceptOrderTimer = object : CountDownTimer(durationInMillis, durationInMillis / 500) {
                override fun onTick(millisUntilFinished: Long) {
                    activity.runOnUiThread {
                        val progress = millisUntilFinished / durationInMillis.toFloat() * 100
                        binding.newOrderProgress.progress = progress
                    }
                }

                override fun onFinish() {
                    binding.newOrderLayout.visibility = View.GONE
                }
            }
        }
    }

    private val fareTypes = listOf("Эконом", "Комфорт", "Бизнесс", "Элит")
    private fun updateInfo(driverName: String, fareType: Int, ordersCount: Int, salary: Int) {
        activity.runOnUiThread {
            toggleWindow(true)

            binding.driverName.text = driverName
            binding.tatifs.text     = String.format("Тариф: %s", fareTypes[fareType])
            binding.salary.text     = String.format("Заработано: %s", Samp.formatter.format(salary))
            binding.countRide.text  = String.format("Поездок: %d", ordersCount)
        }
    }

    private fun newOrder(distance: Float) {
        activity.runOnUiThread {
            toggleWindow(true)
            binding.newOrderDistance.text = formatDistanceAndTime(distance)

            acceptOrderTimer.cancel()

            binding.newOrderLayout.visibility = View.VISIBLE
            binding.acceptOrderLayout.animationXAttention(Attention.ATTENTION_BOUNCE)

            acceptOrderTimer.start()

        }
    }
    private fun toggleWindow(toggle: Boolean) {
        binding.toggleButt.visibility = View.GONE
        if (!toggle) {
            binding.windowLayout.animate()
                ?.translationX(500f)
                ?.setDuration(300)
                ?.alpha(0.0f)
                ?.withEndAction {
                    binding.windowLayout.visibility = View.GONE
                    binding.toggleButt.visibility = View.VISIBLE
                }?.start()

        }
        else {
          //  binding?.windowLayout?.translationX = resources.displayMetrics.widthPixels.toFloat()
            binding.windowLayout.visibility = View.VISIBLE
            binding.windowLayout.animate()
                ?.translationX(0f)
                ?.setDuration(300)
                ?.alpha(1.0f)
                ?.withEndAction {
                    binding.toggleButt.visibility = View.VISIBLE
                }?.start()
        }
    }

    override fun destroy() {
        super.destroy()

        acceptOrderTimer.cancel()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    private fun formatDistanceAndTime(distanceInMeters: Float): String {
        val speedKmPerHour = 70
        val distanceKm = distanceInMeters / 1000.0
        val timeInHours = distanceKm / speedKmPerHour
        var timeInMinutes = (timeInHours * 60).toInt()
        if(timeInMinutes <= 0)
            timeInMinutes = 1

        val formattedDistance = String.format("%.1f km", distanceKm)
        val formattedTime = "$timeInMinutes мин."

        return "Подача: $formattedDistance (~$formattedTime)"
    }

}