package furhatos.app.newskill.flow

import furhatos.app.newskill.flow.partials.YNAnswer
import furhatos.flow.kotlin.FlowControlRunner
import furhatos.flow.kotlin.furhat

fun FlowControlRunner.betterYN(question: String) =
    furhat.askYN(question) {
        include(YNAnswer)
    }
