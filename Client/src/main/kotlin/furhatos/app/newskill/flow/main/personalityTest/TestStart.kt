package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.data.locale.PersonalityTest
import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.flow.utils.betterYN
import furhatos.app.newskill.utils.replaceLast
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val TestStart: State =
    state {
        onEntry {
            if (betterYN(Localization.getLocalizedString("ready_for_questions"))) {
                furhat.say(
                    "${Localization.getLocalizedString(Localization.getLocalizedString("intro_questions"))} ${
                        PersonalityTest
                            .possibleAnswers.values
                            .map{ Localization.getLocalizedString(it) }
                            .reduce { s, t -> "$s$t,\n"}
                            .replaceLast(",", "")
                            .replaceLast(","," ${Localization.getLocalizedString("or_word")}")
                    }",
                )

                if (betterYN("ready_to_start")) {
                    furhat.say("let_s_start")
                    goto(TestCore)
                }
            }
            goto(TestSkip)
        }

        include(Base)
    }
