package furhatos.app.newskill.flow.utils

import furhatos.app.newskill.flow.partials.YNAnswer
import furhatos.flow.kotlin.FlowControlRunner
import furhatos.flow.kotlin.furhat
import furhatos.util.Gender
import furhatos.util.Language

fun FlowControlRunner.betterYN(question: String) =
    furhat.askYN(question) {
        include(YNAnswer)
    }

fun FlowControlRunner.changeOutputLanguage(language: Language) {
    furhat.setVoice(language, Gender.MALE, false)
}
