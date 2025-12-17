package com.lit.game.gui.menu.fragment

import android.annotation.SuppressLint
import android.graphics.Color
import android.os.Bundle
import android.transition.TransitionManager
import android.view.View
import androidx.constraintlayout.widget.ConstraintSet
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.lit.game.R
import com.lit.game.databinding.FragmentMenuMainBinding
import com.lit.game.gui.menu.MenuController
import com.lit.game.gui.menu.MenuViewModel
import com.lit.game.gui.quest.QuestStatus
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

        binding.quest.visibility = View.GONE
        binding.btnQuest.visibility = View.GONE

        vm.time.observe(viewLifecycleOwner) { binding.time.text = "${it ?: ""} ч" }
        vm.level.observe(viewLifecycleOwner) { binding.level.text = (it ?: "").toString() }
        vm.exp.observe(viewLifecycleOwner) { updateExpUI() }
        vm.expMax.observe(viewLifecycleOwner) { updateExpUI() }
        vm.family.observe(viewLifecycleOwner) { binding.family.text = it ?: "" }

        // quest
        vmq.status.observe(viewLifecycleOwner) { raw ->
            val status = QuestStatus.from(raw)

            binding.quest.visibility =
                if (status == QuestStatus.COMPLETED) View.GONE else View.VISIBLE
        }
        vmq.name.observe(viewLifecycleOwner) { binding.nameQuest.text = it ?: "" }
        vmq.description.observe(viewLifecycleOwner) { binding.titleQuest.text = it ?: "" }
        vmq.reward.observe(viewLifecycleOwner) { binding.rewardQuest.text = (it ?: "").toString() }
        vmq.activeQuestCount.observe(viewLifecycleOwner) { rawCount ->
            val raw = rawCount ?: 0
            val c = raw - 1
            if (raw <= 0) {
                binding.btnQuest.visibility = View.GONE
                binding.quest.visibility = View.GONE
            } else {
                binding.quest.visibility = View.VISIBLE

                val showBtn = c > 0
                binding.btnQuest.visibility = if (showBtn) View.VISIBLE else View.GONE
            }

            binding.questCount.text = if ((c) > 0) "+$c" else ""
            updateQuestPlacement(binding.btnQuest.visibility == View.VISIBLE)
        }

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

    @SuppressLint("SetTextI18n")
    private fun updateExpUI() {
        val exp = vm.exp.value ?: 0
        val expMax = vm.expMax.value ?: 0

        binding.exp.text = "$exp / $expMax"

        binding.levelProgress.apply {
            progressMax = expMax.toFloat()
            setProgressWithAnimation(exp.toFloat(), 1000)
        }
    }

    private fun updateQuestPlacement(useBtn: Boolean) {
        val root = binding.mainLayout
        val set = ConstraintSet()
        set.clone(root)

        set.clear(R.id.quest, ConstraintSet.START)
        set.clear(R.id.quest, ConstraintSet.END)
        set.clear(R.id.quest, ConstraintSet.TOP)
        set.clear(R.id.btn_quest, ConstraintSet.START)
        set.clear(R.id.btn_quest, ConstraintSet.END)
        set.clear(R.id.btn_quest, ConstraintSet.TOP)

        val marginTop = resources.getDimensionPixelSize(R.dimen._5sdp)
        val marginEnd = resources.getDimensionPixelSize(R.dimen._5sdp)

        if (useBtn) {
            binding.btnQuest.visibility = View.VISIBLE
            binding.quest.visibility = View.VISIBLE

            set.connect(R.id.btn_quest, ConstraintSet.END, ConstraintSet.PARENT_ID, ConstraintSet.END)
            set.connect(R.id.btn_quest, ConstraintSet.TOP, R.id.imageView4, ConstraintSet.BOTTOM)
            set.setMargin(R.id.btn_quest, ConstraintSet.TOP, marginTop)
            set.setMargin(R.id.btn_quest, ConstraintSet.END, marginEnd)

            set.connect(R.id.quest, ConstraintSet.START, R.id.imageView4, ConstraintSet.START)
            set.connect(R.id.quest, ConstraintSet.TOP, R.id.imageView4, ConstraintSet.BOTTOM)
            set.setMargin(R.id.quest, ConstraintSet.TOP, marginTop)
            set.setMargin(R.id.quest, ConstraintSet.END, marginEnd)
        } else {
            binding.btnQuest.visibility = View.GONE
            binding.quest.visibility = View.VISIBLE

            set.connect(R.id.quest, ConstraintSet.END, ConstraintSet.PARENT_ID, ConstraintSet.END)
            set.connect(R.id.quest, ConstraintSet.TOP, R.id.imageView4, ConstraintSet.BOTTOM)
            set.setMargin(R.id.quest, ConstraintSet.TOP, marginTop)
            set.setMargin(R.id.quest, ConstraintSet.END, marginEnd)
        }

        set.applyTo(root)
    }
}