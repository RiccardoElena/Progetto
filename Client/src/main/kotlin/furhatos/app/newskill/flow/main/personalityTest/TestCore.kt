package furhatos.app.newskill.flow.main.personalityTest

import furhatos.app.newskill.data.locale.PersonalityTest
import furhatos.app.newskill.flow.partials.Base
import furhatos.app.newskill.model.usecases.ComputePersonalityUseCase
import furhatos.flow.kotlin.State
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

            goto(TestEnd(ComputePersonalityUseCase()(responses)))
        }
        include(Base)
    }
