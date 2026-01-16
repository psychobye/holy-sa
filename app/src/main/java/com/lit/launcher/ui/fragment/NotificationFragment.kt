package com.lit.launcher.ui.fragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import androidx.fragment.app.Fragment
import com.lit.game.R

class NotificationFragment : Fragment() {
    private var closeBtn: ImageView? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_notification, container, false)

        closeBtn = view.findViewById(R.id.close_btn)

        onClickInit()
        return view
    }

    private fun onClickInit() {
        closeBtn?.setOnClickListener {
            replaceFragment(MainFragment())
        }
    }

    private fun Fragment.replaceFragment(fragment: Fragment, addToBackStack: Boolean = true) {
        if (!isAdded) return
        parentFragmentManager.beginTransaction().apply {
            setCustomAnimations(
                android.R.anim.fade_in,
                android.R.anim.fade_out,
                android.R.anim.fade_in,
                android.R.anim.fade_out
            )
            replace(R.id.fragment_container, fragment)
            if (addToBackStack) addToBackStack(null)
            commit()
        }
    }
}