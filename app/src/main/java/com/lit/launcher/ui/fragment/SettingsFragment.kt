package com.lit.launcher.ui.fragment

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.animation.Animation
import android.view.animation.AnimationUtils
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.launcher.async.dto.response.FileInfo
import com.lit.launcher.async.task.CacheChecker
import com.lit.launcher.config.Config
import com.lit.launcher.domain.enums.DownloadType
import com.lit.launcher.service.ActivityService
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.storage.NativeStorage
import com.lit.launcher.ui.activity.LoaderActivity
import com.lit.launcher.ui.dialogs.ConfirmDialog
import com.lit.launcher.ui.dialogs.EnterNicknameDialog
import com.lit.launcher.utils.FileUtils
import com.lit.launcher.utils.MainUtils
import com.lit.launcher.utils.MainUtils.Companion.type
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File

class SettingsFragment : Fragment(), View.OnClickListener {
    private var animation: Animation? = null
    private var nicknameField: TextView? = null
    private var activityService: ActivityService? = null
    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        activityService = ActivityServiceImpl()
        val inflate = inflater.inflate(R.layout.fragment_settings, container, false)
        animation = AnimationUtils.loadAnimation(context, R.anim.button_click)
        nicknameField = inflate.findViewById(R.id.nick_edit)
        nicknameField?.setOnClickListener(this)
        initUserData()
        return inflate
    }

    override fun onClick(view: View) {
        when (view.id) {
            R.id.nick_edit -> performNickEditFieldOnClickAction()
            else -> {}
        }
    }

    private fun performNickEditFieldOnClickAction() {
        EnterNicknameDialog(this)
    }

    private fun initUserData() {
        val nickname = NativeStorage.getClientProperty("name", this.activity)
        nicknameField!!.text = nickname
    }

    fun updateNicknameField(nickname: String?) {
        nicknameField!!.text = nickname
    }
}