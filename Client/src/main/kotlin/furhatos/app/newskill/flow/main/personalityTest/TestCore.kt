package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.model.PersonalityTest
import furhatos.app.newskill.model.usecases.ComputePersonalityUseCase
import furhatos.app.newskill.setting.DEBUG_MODE
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state

val TestCore: State =
    state {
        onEntry {
            val questionsToAsk = PersonalityTest.questions.toMutableMap()
            val responses = mutableMapOf<UInt, UInt>()
            while (questionsToAsk.isNotEmpty()) {
                val questionNum = questionsToAsk.keys.random()
                val response =
                    call(
                        AskTestQuestion(questionsToAsk.getValue(questionNum)),
                    ) as UInt
                responses[questionNum] = response
                questionsToAsk.remove(questionNum)
            }

            if (DEBUG_MODE) {
                furhat.say("Il vettore di personalità a te associato è ${ComputePersonalityUseCase()(responses)}")
            }
        }
        include(Base)
    }
