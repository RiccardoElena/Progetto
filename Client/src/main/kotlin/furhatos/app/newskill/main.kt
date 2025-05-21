package furhatos.app.newskill

import furhatos.app.newskill.flow.Init
import furhatos.app.newskill.model.usecases.ComputePersonalityUseCase
import furhatos.flow.kotlin.Flow
import furhatos.skills.Skill

class NewskillSkill : Skill() {
    override fun start() {
        Flow().run(Init)
    }
}

fun main(args: Array<String>) {
    ComputePersonalityUseCase()(
        mapOf(
            1u to 2u,
            2u to 1u,
            3u to 7u,
            4u to 1u,
            5u to 5u,
            6u to 5u,
            7u to 6u,
            8u to 4u,
            9u to 7u,
            10u to 2u,
        ),
    )
    Skill.main(args)
}
