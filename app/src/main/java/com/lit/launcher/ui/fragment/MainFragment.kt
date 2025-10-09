package com.lit.launcher.ui.fragment

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.storage.NativeStorage
import java.io.File

class MainFragment : Fragment() {
    private var playBtn: ImageView? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_main, container, false)

        playBtn = view.findViewById(R.id.play_btn)

        playBtn?.setOnClickListener {
            onClickPlay()
        }

        return view
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