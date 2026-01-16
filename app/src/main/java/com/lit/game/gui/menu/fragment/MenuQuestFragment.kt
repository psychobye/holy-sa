package com.lit.game.gui.menu.fragment

import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.game.databinding.FragmentMenuQuestBinding

class MenuQuestFragment : Fragment(R.layout.fragment_menu_quest) {

    private lateinit var binding: FragmentMenuQuestBinding
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        binding = FragmentMenuQuestBinding.bind(view)
    }
}
