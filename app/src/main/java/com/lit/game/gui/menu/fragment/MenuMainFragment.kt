package com.lit.game.gui.menu.fragment

import android.annotation.SuppressLint
import android.graphics.Color
import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.lit.game.R
import com.lit.game.databinding.FragmentMenuMainBinding
import com.lit.game.gui.menu.MenuController
import com.lit.game.gui.menu.MenuViewModel
import com.lit.game.gui.quest.QuestViewModel
import com.lit.game.gui.util.Utils.addPressScaleAnimation
import com.mikhaellopez.circularprogressbar.CircularProgressBar

class MenuMainFragment : Fragment(R.layout.fragment_menu_main) {
    private val vm by lazy { ViewModelProvider(requireActivity()).get(MenuViewModel::class.java) }
    private val vmq by lazy { ViewModelProvider(requireActivity()).get(QuestViewModel::class.java) }
    var controller: MenuController? = null
    private external fun nativeSendMenuButt(buttId: Int)

    private lateinit var binding: FragmentMenuMainBinding
    @SuppressLint("SetTextI18n")
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        binding = FragmentMenuMainBinding.bind(view)

        vm.time.observe(viewLifecycleOwner) { binding.time.text = "${it ?: ""} ч" }
        vm.level.observe(viewLifecycleOwner) { binding.level.text = (it ?: "").toString() }
        vm.exp.observe(viewLifecycleOwner) { updateExpUI() }
        vm.expMax.observe(viewLifecycleOwner) { updateExpUI() }
        vm.family.observe(viewLifecycleOwner) { binding.family.text = it ?: "" }
        vmq.name.observe(viewLifecycleOwner) { binding.nameQuest.text = it ?: "" }
        vmq.description.observe(viewLifecycleOwner) { binding.titleQuest.text = it ?: "" }
        vmq.reward.observe(viewLifecycleOwner) { binding.rewardQuest.text = (it ?: "").toString() }
        vmq.questid.observe(viewLifecycleOwner) { binding.questCount.text = "+${it ?: ""}" }

        vm.familyColor.observe(viewLifecycleOwner) { colorTriple ->
            colorTriple?.let {
                val (r, g, b) = it
                binding.family.setTextColor(Color.rgb(r, g, b))
            }
        }

        addPressScaleAnimation(binding.btnGarage)
        addPressScaleAnimation(binding.btnSkin)
        addPressScaleAnimation(binding.btnTeleport)
        addPressScaleAnimation(binding.btnTab)
        addPressScaleAnimation(binding.btnSettings)
        addPressScaleAnimation(binding.btnReport)

        binding.btnGarage.setOnClickListener {
            nativeSendMenuButt(1)
            controller?.destroyMenu()
        }
        binding.btnSkin.setOnClickListener {
            nativeSendMenuButt(2)
            controller?.destroyMenu()
        }
        binding.btnTeleport.setOnClickListener {
            nativeSendMenuButt(3)
            controller?.destroyMenu()
        }
        binding.btnTab.setOnClickListener {
            nativeSendMenuButt(4)
            controller?.destroyMenu()
        }
        binding.btnSettings.setOnClickListener {
            nativeSendMenuButt(5)
            controller?.destroyMenu()
        }
        binding.btnReport.setOnClickListener {
            nativeSendMenuButt(6)
            controller?.destroyMenu()
        }
    }

    private fun updateExpUI() {
        val exp = vm.exp.value ?: 0
        val expMax = vm.expMax.value ?: 0

        binding.exp.text = "$exp / $expMax"

        binding.levelProgress.apply {
            progressMax = expMax.toFloat()
            setProgressWithAnimation(exp.toFloat(), 1000)
        }
    }
}