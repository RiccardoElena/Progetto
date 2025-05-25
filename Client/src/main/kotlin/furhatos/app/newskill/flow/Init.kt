package furhatos.app.newskill.flow

import furhatos.app.newskill.flow.main.Greeting
import furhatos.app.newskill.flow.main.Idle
import furhatos.app.newskill.flow.utils.changeOutputLanguage
import furhatos.app.newskill.setting.DISTANCE_TO_ENGAGE
import furhatos.app.newskill.setting.MAX_NUMBER_OF_USERS
import furhatos.app.newskill.setting.SUPPORTED_LANGUAGES
import furhatos.flow.kotlin.State
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.state
import furhatos.flow.kotlin.users

val Init: State =
    state {
        init {
            /** Set our default interaction parameters */
            users.setSimpleEngagementPolicy(DISTANCE_TO_ENGAGE, MAX_NUMBER_OF_USERS)
            furhat.setInputLanguage(*SUPPORTED_LANGUAGES)
            changeOutputLanguage(SUPPORTED_LANGUAGES[0])
        }
        onEntry {
            /** start interaction */
            when {
                furhat.isVirtual() -> goto(Greeting) // Convenient to bypass the need for user when running Virtual Furhat
                users.hasAny() -> {
                    furhat.attend(users.random)
                    goto(Greeting)
                }
                else -> goto(Idle)
            }
        }
    }
