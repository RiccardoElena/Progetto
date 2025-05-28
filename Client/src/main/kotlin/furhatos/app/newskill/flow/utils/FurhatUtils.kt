package furhatos.app.newskill.flow.utils

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.flow.partials.YNAnswer
import furhatos.flow.kotlin.FlowControlRunner
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state
import furhatos.nlu.common.No
import furhatos.nlu.common.Yes
import furhatos.util.Gender
import furhatos.util.Language

fun FlowControlRunner.betterYN(question: String) = call(YN(question)) as Boolean

fun FlowControlRunner.changeOutputLanguage(language: Language) {
    furhat.setVoice(language, Gender.MALE, false)
    Localization.lastLanguage = language
}

@Suppress("ktlint:standard:function-naming")
private fun YN(question: String) =
    state {
        onEntry {
            furhat.ask(Localization.getLocalizedString(question))
        }

        onResponse<Yes> {
            changeOutputLanguage(if (it.language == Language.SPANISH_ES) Language.ITALIAN else it.language)
            terminate(true)
        }

        onResponse<No> {
            changeOutputLanguage(it.language)
            terminate(false)
        }
        include(YNAnswer)
    }
