package com.holy.game.gui.menu.fragment

import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.game.databinding.FragmentMenuShopBinding

class MenuShopFragment : Fragment(R.layout.fragment_menu_shop) {

    private lateinit var binding: FragmentMenuShopBinding
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        binding = FragmentMenuShopBinding.bind(view)

        binding.btnGarage.setOnClickListener { }
    }
}
