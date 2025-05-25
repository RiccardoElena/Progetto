package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.data.locale.PersonalityTest
import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.flow.utils.changeOutputLanguage
import furhatos.app.newskill.nlu.PersonalityTestAnswers
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.state

@Suppress("ktlint:standard:function-naming")
fun AskTestQuestion(question: String) =
    state {
        onEntry {
            furhat.ask(Localization.getLocalizedString(question), endSil = 2000, maxSpeech = 30000)
        }

        onResponse<PersonalityTestAnswers> {
            changeOutputLanguage(it.language)
            if (DEBUG_MODE) {
                furhat.say(
                    "Risposta percepita: ${it.intent.answer}. " + "Risposta registrata: ${PersonalityTest.textAnswerToNum(
                        "${it.intent.answer}",
                    )}",
                )
            }
            terminate(PersonalityTest.textAnswerToNum(it.text))
        }
        include(Base)
    }
