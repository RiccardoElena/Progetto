package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.flow.partials.YNAnswer
import furhatos.app.newskill.model.PersonalityTest
import furhatos.app.newskill.utils.replaceLast
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val TestStart: State =
    state {

        onEntry {
            var resp =
                furhat.askYN("Sei pronto a rispondere a qualche domanda?") {
                    include(YNAnswer)
                }
            if (resp) {
                furhat.say(
                    "Ottimo! Ti saranno poste 10 domande, ad ognuna delle quali potrai rispondere con: ${
                        PersonalityTest
                            .possibleAnswers.values
                            .reduce { s, t -> "$s$t,\n"}
                            .replaceLast(",", "")
                            .replaceLast(","," oppure")
                    }",
                )
                resp =
                    furhat.askYN("Dunque, sei pronto a cominciare?") {
                        include(YNAnswer)
                    }
                if (resp) {
                    furhat.say("Bene! Cominciamo allora. ")
                    goto(TestCore)
                }
            }
            goto(TestSkip)
        }
        include(Base)
    }
