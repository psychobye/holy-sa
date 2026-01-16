package com.lit.game.gui

import android.view.View
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.ChoosespawnBinding

class ChooseSpawn : NativeGui<ChoosespawnBinding>(ChoosespawnBinding::class) {

    private var organization = false
    private var station = false
    private var exit = false
    private var garage = false
    private var house = false

    private var choosedId = 0

    private external fun nativeSendClick(spawnId: Int);

    init {
        activity.runOnUiThread {
            binding.fraction.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (organization) {
                    binding.fraction.setImageResource(R.drawable.spawn_fraction_active)
                    binding.station.setImageResource(R.drawable.spawn_station)
                    binding.lastPos.setImageResource(R.drawable.spawn_exit)
                    binding.garage.setImageResource(R.drawable.spawn_garage)
                    binding.house.setImageResource(R.drawable.spawn_house)
                    choosedId = 1
                }
            }
            binding.station.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (station) {
                    binding.fraction.setImageResource(R.drawable.spawn_fraction)
                    binding.station.setImageResource(R.drawable.spawn_station_active)
                    binding.lastPos.setImageResource(R.drawable.spawn_exit)
                    binding.garage.setImageResource(R.drawable.spawn_garage)
                    binding.house.setImageResource(R.drawable.spawn_house)
                    choosedId = 2
                }
            }
            binding.lastPos.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (exit) {
                    binding.fraction.setImageResource(R.drawable.spawn_fraction)
                    binding.station.setImageResource(R.drawable.spawn_station)
                    binding.lastPos.setImageResource(R.drawable.spawn_exit_active)
                    binding.garage.setImageResource(R.drawable.spawn_garage)
                    binding.house.setImageResource(R.drawable.spawn_house)
                    choosedId = 3
                }
            }
            binding.garage.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (garage) {
                    binding.fraction.setImageResource(R.drawable.spawn_fraction)
                    binding.station.setImageResource(R.drawable.spawn_station)
                    binding.lastPos.setImageResource(R.drawable.spawn_exit)
                    binding.garage.setImageResource(R.drawable.spawn_garage_active)
                    binding.house.setImageResource(R.drawable.spawn_house)
                    choosedId = 4
                }
            }
            binding.house.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (house) {
                    binding.fraction.setImageResource(R.drawable.spawn_fraction)
                    binding.station.setImageResource(R.drawable.spawn_station)
                    binding.lastPos.setImageResource(R.drawable.spawn_exit)
                    binding.garage.setImageResource(R.drawable.spawn_garage)
                    binding.house.setImageResource(R.drawable.spawn_house_active)
                    choosedId = 5
                }
            }
            binding.continueButt.setOnClickListener {
                if (choosedId > 0) {
                    nativeSendClick(choosedId)
                }
            }
        }
    }

    private fun update(organization: Boolean, station: Boolean, exit: Boolean, garage: Boolean, house: Boolean) {
        activity.runOnUiThread {
            this.organization = organization
            this.station = station
            this.exit = exit
            this.garage = garage
            this.house = house
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}