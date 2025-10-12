package com.lit.launcher.ui.fragment

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputMethodManager
import android.widget.EditText
import android.widget.ImageView
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.storage.NativeStorage
import java.io.File

class MainFragment : Fragment() {
    private var playBtn: ImageView? = null
    private var nicknameField: EditText? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_main, container, false)

        playBtn = view.findViewById(R.id.play_btn)
        nicknameField = view.findViewById(R.id.nick_edit_text)

        playBtn?.setOnClickListener {
            onClickPlay()
        }

        initUserData()
        initNicknameListener()
        return view
    }

    private fun initUserData() {
        val nickname = NativeStorage.getClientProperty("name", activity)
        nicknameField?.setText(nickname)
    }

    private fun initNicknameListener() {
        nicknameField?.setOnFocusChangeListener { _, hasFocus ->
            if (!hasFocus) {
                saveNickname()
            }
        }

        nicknameField?.setOnEditorActionListener { v, actionId, event ->
            val isDone = actionId == EditorInfo.IME_ACTION_DONE
            val isEnterKey = event?.keyCode == android.view.KeyEvent.KEYCODE_ENTER && event.action == android.view.KeyEvent.ACTION_DOWN

            if (isDone || isEnterKey) {
                saveNickname()
                nicknameField?.clearFocus()
                hideKeyboard()
                true
            } else {
                false
            }
        }
    }

    private fun saveNickname() {
        val nickname = nicknameField?.text?.toString() ?: ""
        NativeStorage.addClientProperty("name", nickname, activity)
    }

    private fun hideKeyboard() {
        val imm = requireContext().getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
        imm.hideSoftInputFromWindow(nicknameField?.windowToken, 0)
    }

    fun updateNicknameField(nickname: String?) {
        nicknameField?.setText(nickname)
    }

    private fun startGame() {
        val log = File(requireContext().getExternalFilesDir(null), "log.txt")
        log.delete()

        val cinfo = File(requireContext().getExternalFilesDir(null), "CINFO.BIN")
        cinfo.delete()

        val minfo = File(File(requireContext().getExternalFilesDir(null), "models"), "MINFO.BIN")
        minfo.delete()

        val nickname = NativeStorage.getClientProperty("name", requireContext())
        if (nickname.isNullOrBlank()) {
            ActivityServiceImpl.showErrorMessage("Укажите ник!", requireActivity())
            return
        }

        val intent = Intent(requireContext(), Samp::class.java)
        intent.flags = Intent.FLAG_ACTIVITY_CLEAR_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP
        startActivity(intent)

        requireActivity().overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out)

        requireActivity().finish()
    }

    private fun onClickPlay() {
        startGame()
        /*if (isCheckSkipping) {
            startGame()
        } else {
            val progressDialog = findViewById<ConstraintLayout>(R.id.progressDialog)
            progressDialog.visibility = View.VISIBLE

            GlobalScope.launch {
                val filesList = CacheChecker.getInvalidFilesList(this@MainActivity)
                withContext(Dispatchers.Main) {
                    doAfterCacheChecked(filesList)

                    progressDialog.visibility = View.GONE
                }
            }
        }*/
    }
}